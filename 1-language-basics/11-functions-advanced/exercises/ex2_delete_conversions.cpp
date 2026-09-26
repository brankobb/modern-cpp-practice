// KIND: why
// DEMO-OUT: NAIVE speed = 4294967196 rpm
//
// Zadatak 2 -- zašto "= delete" na overload-u (sekcija 5, EMC Item 11)
// Rešenje: exercises/solutions/ex2_delete_conversions.cpp
//
// Motor prima brzinu kao unsigned (rpm ne može biti negativan).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/11-functions-advanced/exercises/ex2_delete_conversions.cpp -DNAIVE
//   setSpeed(-100) prođe bez upozorenja i motor dobije 4294967196 rpm
//   (-100 + 2^32). setSpeed(2.9) postavi 2. Obe implicitne konverzije
//   su legalne, a -Wall -Wextra za njih ne upozoravaju.
// Korak 2: u #else grani napiši setSpeed(unsigned) i spreči sve
//   ostale tipove: template <typename T> void setSpeed(T) = delete;
//   Non-template overload pobeđuje za tačno unsigned, a za sve drugo
//   template je bolji match -- i obrisan je, pa je poziv greška.
//   Proveri: dodaj privremeno setSpeed(-100); i pročitaj grešku
//   ("use of deleted function"), pa ga vrati u komentar.
// Korak 3: kako onda pozivalac koji ima int legalno postavi brzinu?
//   (odgovor: eksplicitno, posle provere -- static_cast<unsigned>(x))

#include <iostream>

#ifdef NAIVE
void setSpeed(unsigned rpm) { std::cout << "speed = " << rpm << " rpm\n"; }

int main() {
    setSpeed(-100);
    setSpeed(2.9);
}
#else
// TODO korak 2

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // setSpeed(1500u);
    // int fromConfig = 1200;
    // if (fromConfig >= 0) setSpeed(static_cast<unsigned>(fromConfig));
}
#endif

/* EXPECTED OUTPUT
speed = 1500 rpm
speed = 1200 rpm
*/
