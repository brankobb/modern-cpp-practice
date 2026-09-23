// VRSTA: zašto
// DEMO-UB: NAIVNO stack-use-after-scope
//
// Zadatak 3 -- zašto [&] za callback koji se čuva za kasnije visi
// (sekcije 5, 6; week2 s09, sekcija 7)
// Rešenje: exercises/solutions/z3_referenca_u_petlji.cpp
//
// U petlji se pravi lista zadataka (callback-ova) koji se izvrše kasnije.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week3-advanced/s14-lambdas/exercises/z3_referenca_u_petlji.cpp -DNAIVNO
//   ASan prijavi stack-use-after-scope. Lambda sa [&] zarobi REFERENCU na
//   id -- promenljivu tela petlje, koja nestaje na kraju svake iteracije.
//   Zadaci se izvršavaju posle petlje, kad nijedan id više ne postoji.
//   [&] je bezbedan samo kad se lambda izvrši dok su promenljive žive
//   (npr. odmah, u std::sort ili std::count_if).
// Korak 2: u #else grani napravi zadatke tako da nose svoju kopiju: [id]
//   ili [=]. Za string koji se samo premešta u lambdu:
//   [ime = std::move(ime)] (sekcija 8).

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main() {
    std::vector<std::function<void()>> zadaci;
    for (int i = 0; i < 3; ++i) {
        int id = 100 + i;
#ifdef NAIVNO
        zadaci.push_back([&] { std::cout << "zadatak " << id << '\n'; });
#else
        // TODO korak 2
        (void)id;
#endif
    }
    for (const auto& z : zadaci) z();
}

/* OČEKIVANI IZLAZ
zadatak 100
zadatak 101
zadatak 102
*/
