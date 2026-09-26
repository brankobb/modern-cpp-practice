// KIND: why
// DEMO-OUT: NAIVE po traženju: [0-9]{2,}
//
// Zadatak 3 -- zašto je kvalitet heša bitan (sekcije 4, 5)
// Rešenje: exercises/solutions/ex3_los_hes.cpp
//
// Mreža 30 x 30 tačaka je u unordered_set-u. Poređenje jednakosti broji
// koliko puta je pozvano, pa se vidi koliko posla košta traženje.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/35-associative-containers/exercises/ex3_los_hes.cpp -DNAIVE
//   Heš (x + y) % 4 daje samo 4 različite vrednosti za 900 tačaka, pa su
//   stotine tačaka u istom bucket-u, i svako traženje ih poredi redom:
//   preko 100 poređenja po traženju (test, libstdc++: 113). Program je
//   tačan -- samo je heš tabela postala lista, O(n) umesto O(1).
// Korak 2: u #else grani napiši dobar heš: kombinuj std::hash<int> za x
//   i y (npr. h ^ (hash(y) + 0x9e3779b9 + (h << 6) + (h >> 2)) -- postupak
//   iz boost::hash_combine). Uslov: jednake tačke daju jednak heš, a
//   različite što ređe isti.

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

#ifdef NAIVE
struct Hes {
    std::size_t operator()(const Tacka& t) const { return static_cast<std::size_t>(t.x + t.y) % 4; }
};
#else
// TODO korak 2 (dok ne napišeš, ovo je naivni heš)
struct Hes {
    std::size_t operator()(const Tacka& t) const { return static_cast<std::size_t>(t.x + t.y) % 4; }
};
#endif

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

/* EXPECTED OUTPUT
nađeno 900 od 900, poređenja po traženju: 1
*/
