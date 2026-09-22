// STD: c++17
// LINK: support/base_value.cpp
// EXPECT-UB: initialization-order-fiasco
// POGREŠNO: globalna promenljiva čija inicijalizacija čita globalnu
//   promenljivu iz DRUGOG .cpp fajla ("static initialization order fiasco").
// Zašto: redosled dinamičke inicijalizacije globalnih promenljivih je određen
//   samo UNUTAR jednog TU-a (redom kojim su napisane). Između TU-ova nije
//   određen ([basic.start.dynamic]). Ovde derived može da se računa pre nego
//   što je base_value.cpp izvršio "int base = compute();", pa pročita 0
//   (zero-init) umesto 41. U praksi zavisi od redosleda fajlova pri linkovanju:
//   isti kod radi ili ne radi u zavisnosti od build skripte.
//   ASan to hvata samo sa check_initialization_order=1 (podešeno dole).
// Ispravno (EC++ Item 4): globalnu promenljivu zameni funkcijom sa static
//   lokalnom promenljivom -- "int& base() { static int b = compute(); return b; }".
//   Lokalni static se inicijalizuje pri PRVOM pozivu (main.cpp, sekcija 7).
//   Ili, ako je vrednost poznata pri kompajliranju: constexpr (C++17) ili
//   constinit (C++20), pa inicijalizacija postaje statička i nema redosleda.
#include <cstdio>

// Uključuje ASan-ovu proveru redosleda inicijalizacije za ovaj program.
extern "C" const char* __asan_default_options() { return "check_initialization_order=1"; }

extern int base;         // definisan u support/base_value.cpp
int derived = base + 1;  // dinamička inicijalizacija čita drugi TU

int main() {
    std::printf("derived = %d (očekivano 42)\n", derived);
}
