// KIND: usage
//
// Zadatak 1 -- map za brojanje, map<string, set<int>> za indeks,
// unordered_map sa sopstvenim hešom (sekcije 1, 3, 4, 5)
//   ./build.sh 7-standard-library/35-associative-containers/exercises/ex1_word_index.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_word_index.cpp
//
// Korak 1: std::map<std::string, int> wordCount(const std::vector<std::string>& lines)
//   -- razbij svaki red na reči (std::istringstream, >>) i prebroj ih.
//   ++m[rec] je ovde upravo ono što treba: nepostojeći ključ počinje od 0.
// Korak 2: std::map<std::string, std::set<int>> indeks(const std::vector<std::string>& redovi)
//   -- za svaku reč, skup brojeva redova (od 1) u kojima se javlja. set
//   sam sortira i uklanja duplikate (reč dvaput u istom redu).
// Korak 3: mapa zauzetosti mreže senzora:
//   std::unordered_map<Position, std::string, PositionHash> -- napiši
//   PositionHash (kombinuj std::hash<int> za x i y) i operator== za Position.

#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Position {
    int x, y;
    // TODO korak 3: operator==
};

// TODO korak 1, 2, 3

int main() {
    std::vector<std::string> lines{"temp rises", "pressure falls temp rises", "temp temp stable"};
    (void)lines;
    // Korak 1 -- otkomentariši:
    // const char* sep = "";
    // for (const auto& [word, n] : wordCount(lines)) {
    //     std::cout << sep << word << '=' << n;
    //     sep = " ";
    // }
    // std::cout << '\n';

    // Korak 2 -- otkomentariši:
    // for (const auto& [word, where] : wordIndex(lines)) {
    //     std::cout << word << ':';
    //     for (int r : where) std::cout << ' ' << r;
    //     std::cout << '\n';
    // }

    // Korak 3 -- otkomentariši:
    // std::unordered_map<Position, std::string, PositionHash> grid{{{0, 0}, "temp"}, {{2, 1}, "humidity"}};
    // grid[{1, 1}] = "pressure";
    // std::cout << "(2, 1): " << grid.at({2, 1}) << ", (1, 1): " << grid.at({1, 1})
    //           << ", occupied (5, 5): " << grid.count({5, 5}) << '\n';
}

/* EXPECTED OUTPUT
falls=1 pressure=1 rises=2 stable=1 temp=4
falls: 2
pressure: 2
rises: 1 2
stable: 3
temp: 1 2 3
(2, 1): humidity, (1, 1): pressure, occupied (5, 5): 0
*/
