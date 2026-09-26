// VRSTA: zašto
// DEMO-OUT: NAIVNO poslato poruka: 0
//
// Zadatak 2 -- zašto bind "ne menja" promenljivu (sekcija 4)
// Rešenje: exercises/solutions/z2_bind_kopira.cpp
//
// posalji(int& brojac, const char* poruka) šalje poruku i uveća brojač
// poslatih. Callback za dugme je napravljen bind-om.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 5-funkcije-kao-vrednosti/31-function-i-bind/exercises/z2_bind_kopira.cpp -DNAIVNO
//   Dugme je kliknuto tri puta, a brojač u main-u je 0. bind KOPIRA sve
//   argumente u sebe (i poslato), i posalji() dobija referencu na tu
//   unutrašnju kopiju -- koju onda uvećava. Nema greške ni upozorenja:
//   int& se lepo veže za kopiju.
// Korak 2: u #else grani napravi callback koji menja PRAVI brojač, na dva
//   načina: a) std::bind sa std::ref(poslato); b) lambda [&poslato].
//   Lambda jasno kaže šta se hvata po referenci, a bind to sakrije.

#include <functional>
#include <iostream>

void posalji(int& brojac, const char* poruka) {
    std::cout << "šaljem: " << poruka << '\n';
    ++brojac;
}

int main() {
    int poslato = 0;
    std::function<void()> naKlik;
#ifdef NAIVNO
    naKlik = std::bind(posalji, poslato, "ping");
#else
    // TODO korak 2
    naKlik = [] {};
#endif
    naKlik();
    naKlik();
    naKlik();
    std::cout << "poslato poruka: " << poslato << '\n';
}

/* OČEKIVANI IZLAZ
šaljem: ping
šaljem: ping
šaljem: ping
poslato poruka: 3
*/
