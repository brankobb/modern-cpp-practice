// VRSTA: upotreba
//
// Zadatak 1 -- erase-remove, insert, reserve i praćenje realokacija
// (sekcije 2, 3, 5)
//   ./build.sh week0-fundamentals/17-vector-initializer-list/exercises/z1_vector_operacije.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_vector_operacije.cpp
//
// Korak 1: void ukloniNegativne(std::vector<int>& v) -- idiom
//   erase-remove: v.erase(std::remove_if(v.begin(), v.end(), uslov), v.end()).
//   remove_if ne briše ništa: premesti elemente koje zadržava na početak i
//   vrati gde počinje "otpad"; erase tek onda skrati vektor.
//   (C++20 ima i std::erase_if(v, uslov) -- ovde ne, vežba radi i u C++17.)
// Korak 2: std::vector<int> spoji(const std::vector<int>& a,
//   const std::vector<int>& b) -- reserve(a.size() + b.size()), pa
//   insert(kraj, b.begin(), b.end()) za oba.
// Korak 3: int brojRealokacija(std::size_t n, bool saReserve) -- dodaj n
//   elemenata sa push_back i broj koliko puta se promenio v.data() posle
//   push_back-a. Sa reserve(n) pre petlje mora biti 0. (Tačan broj bez
//   reserve zavisi od biblioteke -- libstdc++ i libc++ dupliraju kapacitet,
//   MSVC ga povećava 1.5 puta -- zato test ispisuje samo da/ne.)

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

void ispisi(const char* opis, const std::vector<int>& v) {
    std::cout << opis << ':';
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::vector<int> v{3, -1, 4, -1, -5, 9};
    // ukloniNegativne(v);
    // ispisi("bez negativnih", v);

    // Korak 2 -- otkomentariši:
    // ispisi("spojeno", spoji({1, 2}, {3, 4, 5}));

    // Korak 3 -- otkomentariši:
    // std::cout << "bez reserve realocira: " << (brojRealokacija(1000, false) > 0 ? "da" : "ne") << '\n';
    // std::cout << "sa reserve realocira: " << (brojRealokacija(1000, true) > 0 ? "da" : "ne") << '\n';
}

/* OČEKIVANI IZLAZ
bez negativnih: 3 4 9
spojeno: 1 2 3 4 5
bez reserve realocira: da
sa reserve realocira: ne
*/
