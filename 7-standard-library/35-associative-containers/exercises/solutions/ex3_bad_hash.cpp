// Rešenje zadatka ex3_bad_hash.

#include <cstddef>
#include <functional>
#include <iostream>
#include <unordered_set>

long comparisons = 0;

struct Point {
    int x, y;
};

struct Equal {
    bool operator()(const Point& a, const Point& b) const {
        ++comparisons;
        return a.x == b.x && a.y == b.y;
    }
};

// Ako heš daje malo različitih vrednosti (nije dobro): mnogo ključeva
// deli bucket, i traženje postaje linearno.
// Treba ovako: heš koji meša SVA polja, tako da različiti ključevi retko
// dele vrednost. Tada je u bucket-u prosečno oko jedan element (test,
// libstdc++: tačno 1 poređenje po traženju).
struct Hash {
    std::size_t operator()(const Point& t) const {
        std::size_t h = std::hash<int>{}(t.x);
        return h ^ (std::hash<int>{}(t.y) + 0x9e3779b9 + (h << 6) + (h >> 2));
    }
};

int main() {
    std::unordered_set<Point, Hash, Equal> grid;
    for (int x = 0; x < 30; ++x)
        for (int y = 0; y < 30; ++y) grid.insert({x, y});
    comparisons = 0;
    int found = 0;
    for (int x = 0; x < 30; ++x)
        for (int y = 0; y < 30; ++y) found += static_cast<int>(grid.count({x, y}));
    std::cout << "found " << found << " of 900, comparisons per lookup: " << comparisons / 900 << '\n';
}
