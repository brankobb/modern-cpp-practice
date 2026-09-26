// KIND: why
// DEMO-UB: NAIVE stack-use-after-return
//
// Zadatak 3 -- zašto [&] u lambdi koja nadživi funkciju visi
// (sekcija 7, EMC Item 31)
// Rešenje: exercises/solutions/ex3_lambda_referenca.cpp
//
// napraviBrojac() vraća funkciju koja pri svakom pozivu vrati sledeći broj.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/27-forwarding-and-lifetime/exercises/ex3_lambda_referenca.cpp -DNAIVE
//   Lambda hvata lokalnu promenljivu stanje PO REFERENCI, a vraća se iz
//   funkcije -- stanje je nestalo zajedno sa okvirom funkcije. ASan
//   prijavi stack-use-after-return. (Sa g++ 13 na Linux-u je ta provera
//   uključena podrazumevano; ako je tvoj ASan ne radi, pokreni sa
//   ASAN_OPTIONS=detect_stack_use_after_return=1.) Koliko je ovo tiho
//   bez provere: sa ASAN_OPTIONS=detect_stack_use_after_return=0 program
//   "radi" i ispiše smeće (u testu -929173909), bez ikakve poruke.
// Korak 2: u #else grani napiši napraviBrojac() ispravno: stanje uhvati
//   PO VREDNOSTI, sa init-capture ([stanje = 0]) i mutable. Stanje tada
//   živi u samom objektu lambde (lekcija 09, ex3).

#include <functional>
#include <iostream>

#ifdef NAIVE
std::function<int()> napraviBrojac() {
    int stanje = 0;
    return [&stanje] { return ++stanje; };      // referenca na lokalnu
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek vraća 0)
std::function<int()> napraviBrojac() {
    return [] { return 0; };
}
#endif

int main() {
    auto f = napraviBrojac();
    int prvi = f();
    int drugi = f();
    int treci = f();
    std::cout << prvi << ' ' << drugi << ' ' << treci << '\n';
}

/* EXPECTED OUTPUT
1 2 3
*/
