// Rešenje zadatka ex3_los_hes.

#include <cstddef>
#include <functional>
#include <iostream>
#include <unordered_set>

long poredjenja = 0;

struct Tacka {
    int x, y;
};

struct Jednako {
    bool operator()(const Tacka& a, const Tacka& b) const {
        ++poredjenja;
        return a.x == b.x && a.y == b.y;
    }
};

// Ako heš daje malo različitih vrednosti (nije dobro): mnogo ključeva
// deli bucket, i traženje postaje linearno.
// Treba ovako: heš koji meša SVA polja, tako da različiti ključevi retko
// dele vrednost. Tada je u bucket-u prosečno oko jedan element (test,
// libstdc++: tačno 1 poređenje po traženju).
struct Hes {
    std::size_t operator()(const Tacka& t) const {
        std::size_t h = std::hash<int>{}(t.x);
        return h ^ (std::hash<int>{}(t.y) + 0x9e3779b9 + (h << 6) + (h >> 2));
    }
};

int main() {
    std::unordered_set<Tacka, Hes, Jednako> mreza;
    for (int x = 0; x < 30; ++x)
        for (int y = 0; y < 30; ++y) mreza.insert({x, y});
    poredjenja = 0;
    int nadjeno = 0;
    for (int x = 0; x < 30; ++x)
        for (int y = 0; y < 30; ++y) nadjeno += static_cast<int>(mreza.count({x, y}));
    std::cout << "nađeno " << nadjeno << " od 900, poređenja po traženju: " << poredjenja / 900 << '\n';
}
