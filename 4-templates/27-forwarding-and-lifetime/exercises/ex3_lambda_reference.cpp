// KIND: why
// DEMO-UB: NAIVE stack-use-after-return
//
// Zadatak 3 -- zašto [&] u lambdi koja nadživi funkciju visi
// (sekcija 7, EMC Item 31)
// Rešenje: exercises/solutions/ex3_lambda_reference.cpp
//
// makeCounter() vraća funkciju koja pri svakom pozivu vrati sledeći broj.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/27-forwarding-and-lifetime/exercises/ex3_lambda_reference.cpp -DNAIVE
//   Lambda hvata lokalnu promenljivu state PO REFERENCI, a vraća se iz
//   funkcije -- state je nestala zajedno sa okvirom funkcije. ASan
//   prijavi stack-use-after-return. (Sa g++ 13 na Linux-u je ta provera
//   uključena podrazumevano; ako je tvoj ASan ne radi, pokreni sa
//   ASAN_OPTIONS=detect_stack_use_after_return=1.) Koliko je ovo tiho
//   bez provere: sa ASAN_OPTIONS=detect_stack_use_after_return=0 program
//   "radi" i ispiše smeće (u testu -929173909), bez ikakve poruke.
// Korak 2: u #else grani napiši makeCounter() ispravno: state uhvati
//   PO VREDNOSTI, sa init-capture ([state = 0]) i mutable. Stanje tada
//   živi u samom objektu lambde (lekcija 09, ex3).

#include <functional>
#include <iostream>

#ifdef NAIVE
std::function<int()> makeCounter() {
    int state = 0;
    return [&state] { return ++state; };      // referenca na lokalnu
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek vraća 0)
std::function<int()> makeCounter() {
    return [] { return 0; };
}
#endif

int main() {
    auto f = makeCounter();
    int first = f();
    int second = f();
    int third = f();
    std::cout << first << ' ' << second << ' ' << third << '\n';
}

/* EXPECTED OUTPUT
1 2 3
*/
