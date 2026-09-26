// KIND: why
// DEMO-OUT: NAIVE size=0, v\[0\]=0
// DEMO-UB: NAIVE_ASAN container-overflow
//
// Zadatak 2 -- zašto reserve nije resize (sekcije 3, 7)
// Rešenje: exercises/solutions/ex2_reserve_resize.cpp
//
// readSamples(n) treba da vrati vektor sa n očitavanja (ovde: kvadrati).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/07-vector-and-initializer-list/exercises/ex2_reserve_resize.cpp -DNAIVE
//   reserve(n) samo zauzme memoriju (capacity), a size ostane 0. v[i] za
//   i >= size je UB -- ali memorija postoji (unutar capacity), pa ASan
//   ćuti. Vraćeni vektor je prazan, i svi upisi su "nestali".
// Korak 2: isti kod, ali sa anotacijama kontejnera (libstdc++):
//     ./build.sh .../ex2_reserve_resize.cpp -DNAIVE_ASAN
//   (fajl tada sam definiše _GLIBCXX_SANITIZE_VECTOR pre #include).
//   Sada ASan zna gde je kraj ŽIVIH elemenata i prijavi
//   container-overflow. Makro je samo za libstdc++ (g++, i clang na
//   Linux-u). libc++ (npr. MSYS2 clang64 na Windows-u) ga ignoriše i ima
//   sopstvene anotacije -- ovde nije provereno, pa javi da li ASan tamo
//   prijavi grešku već sa -DNAIVE.
// Korak 3: u #else grani napiši readSamples() ispravno, na dva načina:
//   a) reserve(n) + push_back -- jedna alokacija, size raste;
//   b) resize(n) (ili konstruktor std::vector<int>(n)) + v[i] = ... --
//      elementi već postoje (nule), pa je indeksiranje ispravno.

#ifdef NAIVE_ASAN
#define _GLIBCXX_SANITIZE_VECTOR 1
#define NAIVE
#endif

#include <cstddef>
#include <iostream>
#include <vector>

#ifdef NAIVE
std::vector<int> readSamples(std::size_t n) {
    std::vector<int> v;
    v.reserve(n);
    for (std::size_t i = 0; i < n; ++i) v[i] = static_cast<int>(i * i);   // UB: size je 0
    return v;
}

int main() {
    std::vector<int> v = readSamples(4);
    std::cout << "size=" << v.size() << ", v[0]=" << (v.empty() ? 0 : v[0]) << '\n';
}
#else
// TODO korak 3: readSamplesA (reserve + push_back) i readSamplesB (resize)

int main() {
    // Korak 3 -- otkomentariši:
    // for (const auto& v : {readSamplesA(4), readSamplesB(4)}) {
    //     std::cout << "size=" << v.size() << ':';
    //     for (int x : v) std::cout << ' ' << x;
    //     std::cout << '\n';
    // }
}
#endif

/* EXPECTED OUTPUT
size=4: 0 1 4 9
size=4: 0 1 4 9
*/
