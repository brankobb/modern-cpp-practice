// VRSTA: upotreba
//
// Zadatak 1 -- map za brojanje, map<string, set<int>> za indeks,
// unordered_map sa sopstvenim hešom (sekcije 1, 3, 4, 5)
//   ./build.sh week3-advanced/s17-associative-containers/exercises/z1_indeks_reci.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_indeks_reci.cpp
//
// Korak 1: std::map<std::string, int> brojReci(const std::vector<std::string>& redovi)
//   -- razbij svaki red na reči (std::istringstream, >>) i prebroj ih.
//   ++m[rec] je ovde upravo ono što treba: nepostojeći ključ počinje od 0.
// Korak 2: std::map<std::string, std::set<int>> indeks(const std::vector<std::string>& redovi)
//   -- za svaku reč, skup brojeva redova (od 1) u kojima se javlja. set
//   sam sortira i uklanja duplikate (reč dvaput u istom redu).
// Korak 3: mapa zauzetosti mreže senzora:
//   std::unordered_map<Pozicija, std::string, HesPozicije> -- napiši
//   HesPozicije (kombinuj std::hash<int> za x i y) i operator== za Pozicija.

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
    // TODO korak 3: operator==
};

// TODO korak 1, 2, 3

int main() {
    std::vector<std::string> redovi{"temp raste", "pritisak pada temp raste", "temp temp stabilna"};
    (void)redovi;
    // Korak 1 -- otkomentariši:
    // const char* sep = "";
    // for (const auto& [rec, n] : brojReci(redovi)) {
    //     std::cout << sep << rec << '=' << n;
    //     sep = " ";
    // }
    // std::cout << '\n';

    // Korak 2 -- otkomentariši:
    // for (const auto& [rec, gde] : indeks(redovi)) {
    //     std::cout << rec << ':';
    //     for (int r : gde) std::cout << ' ' << r;
    //     std::cout << '\n';
    // }

    // Korak 3 -- otkomentariši:
    // std::unordered_map<Pozicija, std::string, HesPozicije> mreza{{{0, 0}, "temp"}, {{2, 1}, "vlaga"}};
    // mreza[{1, 1}] = "pritisak";
    // std::cout << "(2, 1): " << mreza.at({2, 1}) << ", (1, 1): " << mreza.at({1, 1})
    //           << ", zauzeto (5, 5): " << mreza.count({5, 5}) << '\n';
}

/* OČEKIVANI IZLAZ
pada=1 pritisak=1 raste=2 stabilna=1 temp=4
pada: 2
pritisak: 2
raste: 1 2
stabilna: 3
temp: 1 2 3
(2, 1): vlaga, (1, 1): pritisak, zauzeto (5, 5): 0
*/
