// VRSTA: upotreba
//
// Zadatak 1 -- čitanje brojeva sa proverom, struct kao rezultat, formatiran
// ispis (sekcije 7, 8, 9)
//   ./build.sh 1-osnove-jezika/01-tipovi-ulaz-izlaz-funkcije/exercises/z1_statistika_ulaza.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_statistika_ulaza.cpp
//
// Ulaz dolazi iz std::istringstream (isto bi radilo sa std::cin).
// Korak 1: struct Statistika { int ispravnih; int gresaka; long long zbir;
//   int min; int max; };  (sve na 0)
// Korak 2: Statistika saberi(std::istream& in) -- čita cele brojeve dok
//   ne dođe do kraja ulaza. Neispravan token (slova, broj van opsega
//   int-a) uveća gresaka, pa clear() i ignore() do sledećeg razmaka.
//   Ispravan broj uđe u zbir, min i max. Zbir je long long -- zašto?
//   (Hint: sekcija 4.)
// Korak 3: void ispisi(const Statistika& s) -- tabela sa std::setw:
//   naziv poravnat levo na 12 mesta (std::left), vrednost desno na 12
//   (std::right), a prosek sa 2 decimale (std::fixed, std::setprecision).
//   Nazivi su bez š, č...: setw broji BAJTOVE, a "š" je u UTF-8 dva
//   bajta, pa bi "grešaka" pomerilo poravnanje za jedno mesto.
//   Posle ispisa vrati podrazumevani format (sekcija 8: lepljivi
//   manipulatori).

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

// TODO korak 1, 2, 3

int main() {
    // Korak 1-3 -- otkomentariši:
    // std::istringstream ulaz("12 7 x -3 99999999999 2000000000 2000000000 abc 5");
    // Statistika s = saberi(ulaz);
    // ispisi(s);
    // std::cout << 42 << " (format posle ispisa je vraćen)\n";
}

/* OČEKIVANI IZLAZ
ispravnih              6
neispravnih            3
zbir          4000000021
min                   -3
max           2000000000
prosek      666666670.17
42 (format posle ispisa je vraćen)
*/
