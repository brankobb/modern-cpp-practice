// Rešenje zadatka ex1_indeks_reci.

#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Pozicija {
    int x, y;
    bool operator==(const Pozicija& o) const { return x == o.x && y == o.y; }
};

// Korak 3: heš uz tip; jednaki objekti MORAJU imati jednak heš.
struct HesPozicije {
    std::size_t operator()(const Pozicija& p) const noexcept {
        std::size_t h = std::hash<int>{}(p.x);
        return h ^ (std::hash<int>{}(p.y) + 0x9e3779b9 + (h << 6) + (h >> 2));
    }
};

// Korak 1: map je sortirana po ključu -- ispis je abecedni.
std::map<std::string, int> brojReci(const std::vector<std::string>& redovi) {
    std::map<std::string, int> m;
    for (const auto& red : redovi) {
        std::istringstream is(red);
        std::string rec;
        while (is >> rec) ++m[rec];
    }
    return m;
}

// Korak 2: set<int> kao vrednost -- sortirano, bez duplikata.
std::map<std::string, std::set<int>> indeks(const std::vector<std::string>& redovi) {
    std::map<std::string, std::set<int>> m;
    for (std::size_t i = 0; i < redovi.size(); ++i) {
        std::istringstream is(redovi[i]);
        std::string rec;
        while (is >> rec) m[rec].insert(static_cast<int>(i) + 1);
    }
    return m;
}

int main() {
    std::vector<std::string> redovi{"temp raste", "pritisak pada temp raste", "temp temp stabilna"};
    const char* sep = "";
    for (const auto& [rec, n] : brojReci(redovi)) {
        std::cout << sep << rec << '=' << n;
        sep = " ";
    }
    std::cout << '\n';

    for (const auto& [rec, gde] : indeks(redovi)) {
        std::cout << rec << ':';
        for (int r : gde) std::cout << ' ' << r;
        std::cout << '\n';
    }

    std::unordered_map<Pozicija, std::string, HesPozicije> mreza{{{0, 0}, "temp"}, {{2, 1}, "vlaga"}};
    mreza[{1, 1}] = "pritisak";
    std::cout << "(2, 1): " << mreza.at({2, 1}) << ", (1, 1): " << mreza.at({1, 1})
              << ", zauzeto (5, 5): " << mreza.count({5, 5}) << '\n';
}
