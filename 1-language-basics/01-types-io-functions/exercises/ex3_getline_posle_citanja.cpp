// KIND: why
// DEMO-OUT: NAIVNO senzor 1: id 101, ime \[\]
//
// Zadatak 3 -- zašto getline posle >> pročita prazan red (sekcija 7)
// Rešenje: exercises/solutions/ex3_getline_posle_citanja.cpp
//
// Konfiguracija: u prvom redu broj senzora, pa za svaki: red sa ID-jem i
// red sa imenom (ime može da ima razmake).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/01-types-io-functions/exercises/ex3_getline_posle_citanja.cpp -DNAIVNO
//   Ime prvog senzora je prazno, a dalje sve "klizi". in >> id pročita
//   broj i STANE ispred '\n'. getline zatim čita do prvog '\n' -- a on je
//   odmah tu, pa vrati prazan red. getline i >> drugačije tretiraju
//   prazan prostor: >> ga preskače, getline ne.
// Korak 2: u #else grani napiši ucitaj() ispravno:
//   std::getline(in >> std::ws, ime) -- std::ws preskoči sve praznine
//   (i '\n') pre imena. Proveri i da li je svako čitanje uspelo.

#include <iostream>
#include <sstream>
#include <string>

const char* konfiguracija =
    "2\n"
    "101\n"
    "Senzor temperature motora\n"
    "102\n"
    "Senzor pritiska ulja\n";

#ifdef NAIVNO
void ucitaj(std::istream& in) {
    int n = 0;
    in >> n;
    for (int i = 1; i <= n; ++i) {
        int id = 0;
        std::string ime;
        in >> id;
        std::getline(in, ime);
        std::cout << "senzor " << i << ": id " << id << ", ime [" << ime << "]\n";
    }
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ništa ne čita)
void ucitaj(std::istream&) {}
#endif

int main() {
    std::istringstream in(konfiguracija);
    ucitaj(in);
}

/* EXPECTED OUTPUT
senzor 1: id 101, ime [Senzor temperature motora]
senzor 2: id 102, ime [Senzor pritiska ulja]
*/
