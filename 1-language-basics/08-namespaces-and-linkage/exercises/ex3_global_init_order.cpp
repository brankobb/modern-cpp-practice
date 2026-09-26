// KIND: why
// DEMO-OUT: NAIVE regAddr = 0x10$
//
// Zadatak 3 -- zašto globalna ne sme da zavisi od globalne (sekcija 7, EC++ Item 4)
// Rešenje: exercises/solutions/ex3_global_init_order.cpp
//
// Adresa registra se računa iz bazne adrese periferije. Obe su globalne
// promenljive sa DINAMIČKOM inicijalizacijom (vrednost daje funkcija).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/08-namespaces-and-linkage/exercises/ex3_global_init_order.cpp -DNAIVE
//   Ispiše 0x10 umesto 0x4010, bez greške i bez upozorenja. Pre svake
//   dinamičke inicijalizacije sve globalne su nula (zero-initialization),
//   a u jednom fajlu dinamička ide redom kojim su napisane -- registar se
//   računa pre baze. Kad su u DVA .cpp fajla, redosled nije ni određen
//   (zavisi od redosleda pri linkovanju, ub/u01 u ovoj lekciji).
// Korak 2: u #else grani zameni globalnu bazu funkcijom sa static
//   lokalnom promenljivom: int& base(), pa regAddr = base() + 0x10.
//   Lokalni static se inicijalizuje pri PRVOM pozivu, pa redosled nije bitan.
// Korak 3: ako je baza poznata pri kompajliranju, dovoljno je constexpr:
//   napiši constexpr int baseConst() i
//   constexpr int regAddr2 = baseConst() + 0x10;
//   (statička inicijalizacija -- obavi se pre svake dinamičke).

#include <iostream>

int loadBase() { return 0x4000; }       // npr. čita konfiguraciju pri startu

#ifdef NAIVE
extern int base;
int regAddr = base + 0x10;              // base je ovde još 0
int base = loadBase();

int main() { std::cout << std::hex << "regAddr = 0x" << regAddr << '\n'; }
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // std::cout << std::hex << "regAddr = 0x" << regAddr << '\n';
    // std::cout << "regAddr2 = 0x" << regAddr2 << '\n';
}
#endif

/* EXPECTED OUTPUT
regAddr = 0x4010
regAddr2 = 0x4010
*/
