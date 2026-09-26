// KIND: why
// DEMO-OUT: NAIVE after erasing: 1 -3 4
// DEMO-UB: NAIVE_DEBUG singular iterator
//
// Zadatak 3 -- zašto erase(it) u petlji ne ide uz ++it (sekcija 5)
// Rešenje: exercises/solutions/ex3_erase_in_loop.cpp
//
// Treba obrisati sve negativne elemente iz {1, -2, -3, 4}.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/07-vector-and-initializer-list/exercises/ex3_erase_in_loop.cpp -DNAIVE
//   Ostane -3. erase(it) pomeri ostatak niza za jedno mesto ulevo i
//   poništi it; petlja ga ipak koristi i uradi ++it, pa preskoči element
//   koji je došao na mesto obrisanog. Formalno je to UB (upotreba
//   poništenog iteratora), a u praksi tiho pogrešan rezultat. Da je
//   poslednji element negativan, ++it bi otišao IZA end() i petlja bi
//   čitala van vektora.
// Korak 2: debug režim libstdc++ proverava iteratore:
//     ./build.sh .../ex3_erase_in_loop.cpp -DNAIVE_DEBUG
//   (fajl tada definiše _GLIBCXX_DEBUG pre #include): program stane sa
//   "attempt to increment a singular iterator". _GLIBCXX_DEBUG postoji
//   samo u libstdc++ (g++, i clang na Linux-u); libc++ ga ignoriše.
// Korak 3: u #else grani napiši removeNegatives dva puta:
//   a) petlja sa it = v.erase(it) (erase vraća iterator na sledeći
//      element), a ++it samo kad NIJE brisano;
//   b) erase-remove idiom (ex1) -- kraće i O(n).

#ifdef NAIVE_DEBUG
#define _GLIBCXX_DEBUG 1
#define NAIVE
#endif

#include <algorithm>
#include <iostream>
#include <vector>

void print(const std::vector<int>& v) {
    std::cout << "after erasing:";
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}

#ifdef NAIVE
int main() {
    std::vector<int> v{1, -2, -3, 4};
    for (auto it = v.begin(); it != v.end(); ++it)
        if (*it < 0) v.erase(it);
    print(v);
}
#else
// TODO korak 3: removeNegativesA i removeNegativesB

int main() {
    // Korak 3 -- otkomentariši:
    // std::vector<int> a{1, -2, -3, 4, -5}, b = a;
    // removeNegativesA(a);
    // removeNegativesB(b);
    // print(a);
    // print(b);
}
#endif

/* EXPECTED OUTPUT
after erasing: 1 4
after erasing: 1 4
*/
