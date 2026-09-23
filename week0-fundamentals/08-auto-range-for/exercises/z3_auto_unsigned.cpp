// VRSTA: zašto
// DEMO-UB: NAIVNO heap-buffer-overflow
//
// Zadatak 3 -- auto uzme TAČAN tip inicijalizatora, i kad je unsigned
// (sekcije 3, 6)
// Rešenje: exercises/solutions/z3_auto_unsigned.cpp
//
// Treba ispisati elemente unazad.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/08-auto-range-for/exercises/z3_auto_unsigned.cpp -DNAIVNO
//   "auto i = v.size() - 1" je std::size_t (unsigned), pa je "i >= 0"
//   UVEK tačno. Posle i = 0 dolazi --i = 18446744073709551615, a v[i]
//   čita ispred početka niza (adresa se "okrene"): ASan prijavi
//   heap-buffer-overflow. g++ upozori (-Wtype-limits: "comparison of
//   unsigned expression in '>= 0' is always true"), clang sa -Wall
//   -Wextra ćuti.
//   Šta bi se desilo za PRAZAN vektor, već pri prvom v.size() - 1?
// Korak 2: u #else grani napiši ispisUnazad(const std::vector<int>&) tri
//   puta, svaki ispravan i za prazan vektor:
//   a) reverse iteratori: for (auto it = v.rbegin(); it != v.rend(); ++it)
//   b) indeks, ali uslov pre umanjenja: for (auto i = v.size(); i-- > 0;)
//   c) (samo ideja za C++20: for (int x : std::views::reverse(v)) --
//      ovde ne pišeš, jer vežba mora da radi i u C++17)

#include <iostream>
#include <vector>

#ifdef NAIVNO
void ispisUnazad(const std::vector<int>& v) {
    for (auto i = v.size() - 1; i >= 0; --i) std::cout << v[i] << ' ';
    std::cout << '\n';
}
#endif

// TODO korak 2: ispisUnazadA (iteratori) i ispisUnazadB (indeks)

int main() {
    std::vector<int> v{1, 2, 3};
    std::vector<int> prazan;
    (void)v;
    (void)prazan;
#ifdef NAIVNO
    ispisUnazad(v);
#endif
    // Korak 2 -- otkomentariši:
    // ispisUnazadA(v);
    // ispisUnazadB(v);
    // ispisUnazadA(prazan);
    // ispisUnazadB(prazan);
}

/* OČEKIVANI IZLAZ
a: 3 2 1
b: 3 2 1
a:
b:
*/
