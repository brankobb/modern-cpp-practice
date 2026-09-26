// KIND: why
// DEMO-OUT: NAIVNO vlaga: nije pronađen
//
// Zadatak 3 -- zašto CTAD od string literala ne daje std::string (sekcija 1)
// Rešenje: exercises/solutions/ex3_ctad_literal.cpp
//
// Tabela kanala se pravi CTAD-om, a ime kanala stiže iz bafera (kao iz
// C API-ja ili sa serijskog porta).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/29-cpp17-templates/exercises/ex3_ctad_literal.cpp -DNAIVNO
//   Nijedan kanal nije pronađen, a oba su u tabeli. std::pair{"temp", 21} je
//   pair<const char*, int> -- CTAD uzme tip literala posle "raspadanja"
//   niza u pokazivač. Mapa je zato std::map<const char*, int>: ključevi
//   se porede kao POKAZIVAČI (adrese), ne kao tekst. Bafer ima drugu
//   adresu od literala. Bez greške i bez upozorenja.
//   (Pretraga literalom, find("vlaga"), ovde slučajno uspe: kompajler je oba
//   pojavljivanja istog literala smestio na istu adresu. Standard to ne
//   garantuje.)
// Korak 2: u #else grani napravi tabelu tako da ključ bude std::string --
//   literal sa sufiksom s (using namespace std::string_literals), ili
//   eksplicitan tip std::map<std::string, int>.

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#ifdef NAIVNO
auto napraviTabelu() { return std::map{std::pair{"temp", 21}, std::pair{"vlaga", 40}}; }
#else
// TODO korak 2 (dok ne napišeš, tabela je prazna)
auto napraviTabelu() { return std::map<std::string, int>{}; }
#endif

int main() {
    auto tabela = napraviTabelu();
    char ulaz[16];
    for (const char* ime : {"temp", "vlaga"}) {
        std::strcpy(ulaz, ime);                  // ime stiglo "spolja", u bafer
        auto it = tabela.find(ulaz);
        std::cout << ulaz << ": ";
        if (it == tabela.end())
            std::cout << "nije pronađen\n";
        else
            std::cout << it->second << '\n';
    }
}

/* EXPECTED OUTPUT
temp: 21
vlaga: 40
*/
