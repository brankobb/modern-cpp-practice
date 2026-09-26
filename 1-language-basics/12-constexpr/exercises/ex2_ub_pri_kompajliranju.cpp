// KIND: why
// DEMO-UB: NAIVNO signed integer overflow
// DEMO-ERR: CONSTEXPR overflow in constant expression|outside the range of representable values
//
// Zadatak 2 -- zašto je constexpr i alat za hvatanje UB-a (sekcija 2)
// Rešenje: exercises/solutions/ex2_ub_pri_kompajliranju.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/12-constexpr/exercises/ex2_ub_pri_kompajliranju.cpp -DNAIVNO
//   fakt(13) = 6227020800 ne staje u int. Prekoračenje signed int-a je UB.
//   Bez UBSan-a bi program ispisao neki broj i nastavio; UBSan ga uhvati
//   tek kad se TA linija izvrši.
// Korak 2: ista funkcija, ali rezultat traži kao constexpr promenljivu:
//     ./build.sh .../ex2_ub_pri_kompajliranju.cpp -DCONSTEXPR
//   Pri izračunavanju konstantnog izraza UB nije dozvoljen
//   ([expr.const]), pa kompajler ODBIJE program: g++ "overflow in
//   constant expression", clang "value 6227020800 is outside the range
//   of representable values of type 'int'".
// Korak 3: u #else grani napiši constexpr long long fakt(int n) i
//   static_assert testove za fakt(0), fakt(5) i fakt(20). Probaj
//   static_assert(fakt(21) > 0) -- šta kaže kompajler? (vrati u komentar)

#include <iostream>

#if defined(NAIVNO) || defined(CONSTEXPR)
constexpr int fakt(int n) { return n <= 1 ? 1 : n * fakt(n - 1); }

int main() {
#ifdef CONSTEXPR
    constexpr int f = fakt(13);
#else
    int f = fakt(13);
#endif
    std::cout << "13! = " << f << '\n';
}
#else
// TODO korak 3

int main() {
    // Korak 3 -- otkomentariši:
    // std::cout << "13! = " << fakt(13) << '\n';
    // std::cout << "20! = " << fakt(20) << '\n';
}
#endif

/* EXPECTED OUTPUT
13! = 6227020800
20! = 2432902008176640000
*/
