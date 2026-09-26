// VRSTA: zašto
// DEMO-OUT: NAIVNO brzina = 4294967196 rpm
//
// Zadatak 2 -- zašto "= delete" na overload-u (sekcija 5, EMC Item 11)
// Rešenje: exercises/solutions/z2_delete_konverzije.cpp
//
// Motor prima brzinu kao unsigned (rpm ne može biti negativan).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-osnove-jezika/11-funkcije-napredno/exercises/z2_delete_konverzije.cpp -DNAIVNO
//   postaviBrzinu(-100) prođe bez upozorenja i motor dobije 4294967196 rpm
//   (-100 + 2^32). postaviBrzinu(2.9) postavi 2. Obe implicitne konverzije
//   su legalne, a -Wall -Wextra za njih ne upozoravaju.
// Korak 2: u #else grani napiši postaviBrzinu(unsigned) i spreči sve
//   ostale tipove: template <typename T> void postaviBrzinu(T) = delete;
//   Non-template overload pobeđuje za tačno unsigned, a za sve drugo
//   template je bolji match -- i obrisan je, pa je poziv greška.
//   Proveri: dodaj privremeno postaviBrzinu(-100); i pročitaj grešku
//   ("use of deleted function"), pa ga vrati u komentar.
// Korak 3: kako onda pozivalac koji ima int legalno postavi brzinu?
//   (odgovor: eksplicitno, posle provere -- static_cast<unsigned>(x))

#include <iostream>

#ifdef NAIVNO
void postaviBrzinu(unsigned rpm) { std::cout << "brzina = " << rpm << " rpm\n"; }

int main() {
    postaviBrzinu(-100);
    postaviBrzinu(2.9);
}
#else
// TODO korak 2

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // postaviBrzinu(1500u);
    // int izKonfiguracije = 1200;
    // if (izKonfiguracije >= 0) postaviBrzinu(static_cast<unsigned>(izKonfiguracije));
}
#endif

/* OČEKIVANI IZLAZ
brzina = 1500 rpm
brzina = 1200 rpm
*/
