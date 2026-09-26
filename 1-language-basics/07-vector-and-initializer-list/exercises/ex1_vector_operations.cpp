// KIND: usage
//
// Zadatak 1 -- erase-remove, insert, reserve i praćenje realokacija
// (sekcije 2, 3, 5)
//   ./build.sh 1-language-basics/07-vector-and-initializer-list/exercises/ex1_vector_operations.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_vector_operations.cpp
//
// Korak 1: void removeNegatives(std::vector<int>& v) -- idiom
//   erase-remove: v.erase(std::remove_if(v.begin(), v.end(), uslov), v.end()).
//   remove_if ne briše ništa: premesti elemente koje zadržava na početak i
//   vrati gde počinje "otpad"; erase tek onda skrati vektor.
//   (C++20 ima i std::erase_if(v, uslov) -- ovde ne, vežba radi i u C++17.)
// Korak 2: std::vector<int> concat(const std::vector<int>& a,
//   const std::vector<int>& b) -- reserve(a.size() + b.size()), pa
//   insert(kraj, b.begin(), b.end()) za oba.
// Korak 3: int countReallocations(std::size_t n, bool withReserve) -- dodaj
//   n elemenata sa push_back i broj koliko puta se promenio v.data() posle
//   push_back-a. Sa reserve(n) pre petlje mora biti 0. (Tačan broj bez
//   reserve zavisi od biblioteke -- libstdc++ i libc++ dupliraju kapacitet,
//   MSVC ga povećava 1.5 puta -- zato test ispisuje samo yes/no.)

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

void print(const char* label, const std::vector<int>& v) {
    std::cout << label << ':';
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::vector<int> v{3, -1, 4, -1, -5, 9};
    // removeNegatives(v);
    // print("without negatives", v);

    // Korak 2 -- otkomentariši:
    // print("concatenated", concat({1, 2}, {3, 4, 5}));

    // Korak 3 -- otkomentariši:
    // std::cout << "reallocates without reserve: " << (countReallocations(1000, false) > 0 ? "yes" : "no") << '\n';
    // std::cout << "reallocates with reserve: " << (countReallocations(1000, true) > 0 ? "yes" : "no") << '\n';
}

/* EXPECTED OUTPUT
without negatives: 3 4 9
concatenated: 1 2 3 4 5
reallocates without reserve: yes
reallocates with reserve: no
*/
