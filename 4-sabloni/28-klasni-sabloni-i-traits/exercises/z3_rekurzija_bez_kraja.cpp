// VRSTA: zašto
// DEMO-ERR: NAIVNO no matching function for call to 'ispisi
//
// Zadatak 3 -- zašto variadic rekurzija mora da ima kraj, i zašto je fold
// jednostavniji (sekcija 2)
// Rešenje: exercises/solutions/z3_rekurzija_bez_kraja.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-sabloni/28-klasni-sabloni-i-traits/exercises/z3_rekurzija_bez_kraja.cpp -DNAIVNO
//   Greška pri KOMPAJLIRANJU: "no matching function for call to 'ispisi()'".
//   "Rekurzija" ovde nije poziv pri izvršavanju, nego lanac
//   instancijacija: ispisi<int, double, const char*> treba ispisi<double,
//   const char*>, ta treba ispisi<const char*>, a ta treba ispisi() -- bez
//   argumenata. Šablon traži bar jedan (Prvi), pa ispisi() ne postoji.
//   Uslov "if (sizeof...(ostali) > 0)" ne bi pomogao: obični if ne sprečava
//   da se poziv u njemu KOMPAJLIRA.
// Korak 2: u #else grani napiši tri ispravne verzije:
//   a) ispisiA: dodaj osnovni slučaj -- ne-šablon void ispisiA() koji
//      ispiše '\n' (C++11 način);
//   b) ispisiB: if constexpr (sizeof...(ostali) > 0) oko rekurzivnog
//      poziva -- grana se ne instancira kad je uslov false (C++17);
//   c) ispisiC: bez rekurzije, fold izraz preko zareza (C++17).

#include <iostream>

#ifdef NAIVNO
template <typename Prvi, typename... Ostali>
void ispisi(const Prvi& p, const Ostali&... ostali) {
    std::cout << p << ' ';
    ispisi(ostali...);
}

int main() { ispisi(1, 2.5, "tri"); }
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // std::cout << "a: ";
    // ispisiA(1, 2.5, "tri");
    // std::cout << "b: ";
    // ispisiB(1, 2.5, "tri");
    // std::cout << "c: ";
    // ispisiC(1, 2.5, "tri");
}
#endif

/* OČEKIVANI IZLAZ
a: 1 2.5 tri
b: 1 2.5 tri
c: 1 2.5 tri
*/
