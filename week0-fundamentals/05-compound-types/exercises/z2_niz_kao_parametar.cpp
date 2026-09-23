// VRSTA: zašto
// DEMO-OUT: NAIVNO elemenata: 2, zbir: 3
//
// Zadatak 2 -- zašto C niz "zaboravi" veličinu kad ga proslediš (sekcije 2, 3)
// Rešenje: exercises/solutions/z2_niz_kao_parametar.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/05-compound-types/exercises/z2_niz_kao_parametar.cpp -DNAIVNO
//   Niz ima 10 elemenata, a funkcija vidi 2. Pročitaj upozorenje
//   kompajlera (-Wsizeof-array-argument). Parametar "int niz[10]" je u
//   stvari "int* niz" -- [10] se ignoriše ([dcl.fct]: niz kao parametar se
//   prilagođava u pokazivač). sizeof(int*) / sizeof(int) = 8 / 4 = 2.
// Korak 2: napiši template <std::size_t N> int zbirRef(const int (&niz)[N])
//   -- referenca na niz NE raspada se, pa N kompajler izvede sam.
// Korak 3: napiši int zbirArray(const std::array<int, 10>& niz) -- isto,
//   sa std::array. Otkomentariši test.

#include <array>
#include <cstddef>
#include <iostream>

#ifdef NAIVNO
int zbir(int niz[10]) {
    std::size_t n = sizeof(niz) / sizeof(niz[0]);   // sizeof POKAZIVAČA!
    int s = 0;
    for (std::size_t i = 0; i < n; ++i) s += niz[i];
    std::cout << "elemenata: " << n << ", zbir: " << s << '\n';
    return s;
}
#endif

// TODO korak 2 i 3

int main() {
    int niz[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    (void)niz;
#ifdef NAIVNO
    zbir(niz);
#endif
    // Korak 2 -- otkomentariši:
    // std::cout << "zbirRef: " << zbirRef(niz) << '\n';

    // Korak 3 -- otkomentariši:
    // std::array<int, 10> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    // std::cout << "zbirArray: " << zbirArray(a) << '\n';
}

/* OČEKIVANI IZLAZ
zbirRef: 55
zbirArray: 55
*/
