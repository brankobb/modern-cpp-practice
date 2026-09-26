// VRSTA: zašto
// DEMO-OUT: NAIVNO alarm postavljen na 9h
//
// Zadatak 3 -- zašto bind računa argumente ODMAH, a lambda pri pozivu
// (sekcija 5, EMC Item 34)
// Rešenje: exercises/solutions/z3_bind_racuna_odmah.cpp
//
// "Odloži alarm": kad korisnik pritisne dugme, alarm treba da zvoni sat
// vremena od TRENUTKA PRITISKA.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 5-funkcije-kao-vrednosti/31-function-i-bind/exercises/z3_bind_racuna_odmah.cpp -DNAIVNO
//   Callback je napravljen u 8h, dugme je pritisnuto u 12h, a alarm je
//   postavljen na 9h. Izraz trenutnoVreme() + 1 je argument bind-a, pa se
//   izračunao kad je bind POZVAN (u 8h), a ne kad je callback pozvan.
// Korak 2: u #else grani napiši callback kao lambdu koja računa vreme u
//   svom telu -- izraz u telu lambde se izvršava pri svakom pozivu.

#include <functional>
#include <iostream>

int sat = 8;
int trenutnoVreme() { return sat; }
void postaviAlarm(int kada) { std::cout << "alarm postavljen na " << kada << "h\n"; }

int main() {
    std::function<void()> odlozi;
#ifdef NAIVNO
    odlozi = std::bind(postaviAlarm, trenutnoVreme() + 1);
#else
    // TODO korak 2
    odlozi = [] {};
#endif
    sat = 12;              // ...četiri sata kasnije, korisnik pritisne dugme
    odlozi();
}

/* OČEKIVANI IZLAZ
alarm postavljen na 13h
*/
