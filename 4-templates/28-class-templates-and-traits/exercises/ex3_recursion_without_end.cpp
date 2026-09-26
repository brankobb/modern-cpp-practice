// KIND: why
// DEMO-ERR: NAIVE no matching function for call to 'print
//
// Zadatak 3 -- zašto variadic rekurzija mora da ima kraj, i zašto je fold
// jednostavniji (sekcija 2)
// Rešenje: exercises/solutions/ex3_recursion_without_end.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/28-class-templates-and-traits/exercises/ex3_recursion_without_end.cpp -DNAIVE
//   Greška pri KOMPAJLIRANJU: "no matching function for call to 'print()'".
//   "Rekurzija" ovde nije poziv pri izvršavanju, nego lanac
//   instancijacija: print<int, double, const char*> treba print<double,
//   const char*>, ta treba print<const char*>, a ta treba print() -- bez
//   argumenata. Šablon traži bar jedan (First), pa print() ne postoji.
//   Uslov "if (sizeof...(rest) > 0)" ne bi pomogao: obični if ne sprečava
//   da se poziv u njemu KOMPAJLIRA.
// Korak 2: u #else grani napiši tri ispravne verzije:
//   a) printA: dodaj osnovni slučaj -- ne-šablon void printA() koji
//      ispiše '\n' (C++11 način);
//   b) printB: if constexpr (sizeof...(rest) > 0) oko rekurzivnog
//      poziva -- grana se ne instancira kad je uslov false (C++17);
//   c) printC: bez rekurzije, fold izraz preko zareza (C++17).

#include <iostream>

#ifdef NAIVE
template <typename First, typename... Rest>
void print(const First& p, const Rest&... rest) {
    std::cout << p << ' ';
    print(rest...);
}

int main() { print(1, 2.5, "three"); }
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // std::cout << "a: ";
    // printA(1, 2.5, "three");
    // std::cout << "b: ";
    // printB(1, 2.5, "three");
    // std::cout << "c: ";
    // printC(1, 2.5, "three");
}
#endif

/* EXPECTED OUTPUT
a: 1 2.5 three
b: 1 2.5 three
c: 1 2.5 three
*/
