// KIND: why
// DEMO-OUT: NAIVE sensor 1: id 101, name \[\]
//
// Zadatak 3 -- zašto getline posle >> pročita prazan red (sekcija 7)
// Rešenje: exercises/solutions/ex3_getline_after_extraction.cpp
//
// Konfiguracija: u prvom redu broj senzora, pa za svaki: red sa ID-jem i
// red sa imenom (ime može da ima razmake).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/01-types-io-functions/exercises/ex3_getline_after_extraction.cpp -DNAIVE
//   Ime prvog senzora je prazno, a dalje sve "klizi". in >> id pročita
//   broj i STANE ispred '\n'. getline zatim čita do prvog '\n' -- a on je
//   odmah tu, pa vrati prazan red. getline i >> drugačije tretiraju
//   prazan prostor: >> ga preskače, getline ne.
// Korak 2: u #else grani napiši load() ispravno:
//   std::getline(in >> std::ws, name) -- std::ws preskoči sve praznine
//   (i '\n') pre imena. Proveri i da li je svako čitanje uspelo.

#include <iostream>
#include <sstream>
#include <string>

const char* config =
    "2\n"
    "101\n"
    "Engine temperature sensor\n"
    "102\n"
    "Oil pressure sensor\n";

#ifdef NAIVE
void load(std::istream& in) {
    int n = 0;
    in >> n;
    for (int i = 1; i <= n; ++i) {
        int id = 0;
        std::string name;
        in >> id;
        std::getline(in, name);
        std::cout << "sensor " << i << ": id " << id << ", name [" << name << "]\n";
    }
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ništa ne čita)
void load(std::istream&) {}
#endif

int main() {
    std::istringstream in(config);
    load(in);
}

/* EXPECTED OUTPUT
sensor 1: id 101, name [Engine temperature sensor]
sensor 2: id 102, name [Oil pressure sensor]
*/
