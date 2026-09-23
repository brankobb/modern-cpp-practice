// VRSTA: zašto
// DEMO-OUT: NAIVNO registar = 0x10$
//
// Zadatak 3 -- zašto globalna ne sme da zavisi od globalne (sekcija 7, EC++ Item 4)
// Rešenje: exercises/solutions/z3_redosled_globalnih.cpp
//
// Adresa registra se računa iz bazne adrese periferije. Obe su globalne
// promenljive sa DINAMIČKOM inicijalizacijom (vrednost daje funkcija).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/06-namespaces-linkage/exercises/z3_redosled_globalnih.cpp -DNAIVNO
//   Ispiše 0x10 umesto 0x4010, bez greške i bez upozorenja. Pre svake
//   dinamičke inicijalizacije sve globalne su nula (zero-initialization),
//   a u jednom fajlu dinamička ide redom kojim su napisane -- registar se
//   računa pre baze. Kad su u DVA .cpp fajla, redosled nije ni određen
//   (zavisi od redosleda pri linkovanju, ub/u01 u ovoj lekciji).
// Korak 2: u #else grani zameni globalnu bazu funkcijom sa static
//   lokalnom promenljivom: int& baza(), pa registar = baza() + 0x10.
//   Lokalni static se inicijalizuje pri PRVOM pozivu, pa redosled nije bitan.
// Korak 3: ako je baza poznata pri kompajliranju, dovoljno je constexpr:
//   napiši constexpr int bazaKonst() i
//   constexpr int registar2 = bazaKonst() + 0x10;
//   (statička inicijalizacija -- obavi se pre svake dinamičke).

#include <iostream>

int ucitajBazu() { return 0x4000; }     // npr. čita konfiguraciju pri startu

#ifdef NAIVNO
extern int baza;
int registar = baza + 0x10;             // baza je ovde još 0
int baza = ucitajBazu();

int main() { std::cout << std::hex << "registar = 0x" << registar << '\n'; }
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // std::cout << std::hex << "registar = 0x" << registar << '\n';
    // std::cout << "registar2 = 0x" << registar2 << '\n';
}
#endif

/* OČEKIVANI IZLAZ
registar = 0x4010
registar2 = 0x4010
*/
