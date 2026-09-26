// KIND: why
// DEMO-UB: NAIVE heap-buffer-overflow|addition of unsigned offset to 0x[0-9a-f]+ overflowed
//
// Zadatak 3 -- auto uzme TAČAN tip inicijalizatora, i kad je unsigned
// (sekcije 3, 6)
// Rešenje: exercises/solutions/ex3_auto_unsigned.cpp
//
// Treba ispisati elemente unazad.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/10-auto-and-range-for/exercises/ex3_auto_unsigned.cpp -DNAIVE
//   "auto i = v.size() - 1" je std::size_t (unsigned), pa je "i >= 0"
//   UVEK tačno. Posle i = 0 dolazi --i = 18446744073709551615, a v[i]
//   čita ispred početka niza (adresa se "okrene"): ASan prijavi
//   heap-buffer-overflow. (Sa clang-om UBSan to uhvati korak ranije, već
//   pri računanju adrese u operator[]: "addition of unsigned offset ...
//   overflowed".) g++ upozori (-Wtype-limits: "comparison of
//   unsigned expression in '>= 0' is always true"), clang sa -Wall
//   -Wextra ćuti.
//   Šta bi se desilo za PRAZAN vektor, već pri prvom v.size() - 1?
// Korak 2: u #else grani napiši printReversed(const std::vector<int>&) tri
//   puta, svaki ispravan i za prazan vektor:
//   a) reverse iteratori: for (auto it = v.rbegin(); it != v.rend(); ++it)
//   b) indeks, ali uslov pre umanjenja: for (auto i = v.size(); i-- > 0;)
//   c) (samo ideja za C++20: for (int x : std::views::reverse(v)) --
//      ovde ne pišeš, jer vežba mora da radi i u C++17)

#include <iostream>
#include <vector>

#ifdef NAIVE
void printReversed(const std::vector<int>& v) {
    for (auto i = v.size() - 1; i >= 0; --i) std::cout << v[i] << ' ';
    std::cout << '\n';
}
#endif

// TODO korak 2: printReversedA (iteratori) i printReversedB (indeks)

int main() {
    std::vector<int> v{1, 2, 3};
    std::vector<int> empty;
    (void)v;
    (void)empty;
#ifdef NAIVE
    printReversed(v);
#endif
    // Korak 2 -- otkomentariši:
    // printReversedA(v);
    // printReversedB(v);
    // printReversedA(empty);
    // printReversedB(empty);
}

/* EXPECTED OUTPUT
a: 3 2 1
b: 3 2 1
a:
b:
*/
