// Završna vežba dela 1 -- izveštaj o merenjima
//   ./build.sh 1-language-basics/capstone/task.cpp
// Uputstvo i spisak lekcija: 1-language-basics/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav: funkcije postoje, ali ne rade ništa. Piši
// ih redom; posle svakog koraka pokreni i vidi koliko izveštaja već valja.
//
// Korak 1: procitajBroj i parsirajKanal
//   -- ceo tekst mora da bude broj ("2x" nije); std::istringstream, >>, pa
//   proveri da ništa nije ostalo (lekcija 06, sekcija 5; lekcija 01,
//   sekcija 7).
//   -- red "kanal temp min=-20 max=60 jedinica=C": reči preko >>, par
//   ključ=vrednost preko find('=') i substr (lekcija 06, sekcija 3). Kanal
//   bez min ili max, ili sa min >= max, nije ispravan.
// Korak 2: nadjiKanal, obradiMerenje, ucitaj
//   -- std::getline red po red; prazni redovi i redovi sa '#' se
//   preskaču; red koji počinje sa "kanal " je konfiguracija, ostali su
//   merenja "ime vrednost". Merenje može da ima tačno jednu grešku, tim
//   redom: loš format, nepoznat kanal, nije broj, van opsega -- broji ih
//   preko zabeleziGresku.
//   -- parametri: šta se samo čita ide kao const&, šta se menja kao &
//   (lekcija 04, sekcija 10; lekcija 09, sekcija 3).
// Korak 3: statistika
//   -- za svaki kanal n, min, max, zbir; range-for sa structured bindings
//   preko p.merenja (lekcija 10, sekcija 7).
// Korak 4: kolona (dva overload-a) i ispisiIzvestaj
//   -- std::setw, std::left/std::right, std::fixed, std::setprecision(1)
//   (lekcija 01, sekcija 8); kanal bez merenja ima "-" u kolonama;
//   overload za double i za std::string (lekcija 11, sekcija 1).

#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace merenja {

struct Kanal {
    std::string ime;
    double min = 0;
    double max = 0;
    std::string jedinica;
};

struct Merenje {
    std::size_t kanal;   // indeks u vektoru kanala
    double vrednost;
};

enum class Greska { NepoznatKanal, NijeBroj, VanOpsega, LosFormat };

constexpr std::array<const char*, 4> kNaziviGresaka{"nepoznat kanal", "nije broj", "van opsega", "loš format"};
static_assert(kNaziviGresaka.size() == static_cast<std::size_t>(Greska::LosFormat) + 1,
              "tabela naziva mora da prati enum Greska");

constexpr const char* naziv(Greska g) { return kNaziviGresaka[static_cast<std::size_t>(g)]; }

struct Podaci {
    std::vector<Kanal> kanali;
    std::vector<Merenje> merenja;
    std::array<int, kNaziviGresaka.size()> greske{};   // brojač po vrsti greške
    int redovaMerenja = 0;
    int losihRedovaKonfiguracije = 0;
};

// ---------------------------------------------------------------- korak 1
// TODO: true ako je ceo tekst broj; vrednost upisuje u izlaz.
bool procitajBroj(const std::string&, double&) { return false; }

// TODO: true ako je red ispravna konfiguracija kanala; popunjava k.
bool parsirajKanal(const std::string&, Kanal&) { return false; }

// ---------------------------------------------------------------- korak 2
// TODO: indeks kanala sa tim imenom, ili -1.
int nadjiKanal(const std::vector<Kanal>&, const std::string&) { return -1; }

void zabeleziGresku(Podaci& p, Greska g) { ++p.greske[static_cast<std::size_t>(g)]; }

// TODO: jedan red merenja -- ili ispravno merenje u p.merenja, ili tačno
// jedna greška; u oba slučaja ++p.redovaMerenja.
void obradiMerenje(Podaci&, const std::string&) {}

// TODO: ceo ulaz, red po red.
Podaci ucitaj(std::istream&) { return {}; }

// ---------------------------------------------------------------- korak 3
struct Statistika {
    int n = 0;
    double min = 0, max = 0, zbir = 0;
};

// TODO: jedna Statistika po kanalu, istim redom kao p.kanali.
std::vector<Statistika> statistika(const Podaci& p) { return std::vector<Statistika>(p.kanali.size()); }

// ---------------------------------------------------------------- korak 4
// TODO: dva overload-a funkcije kolona (double i std::string) i izveštaj.
void ispisiIzvestaj(std::ostream&, const Podaci&) {}

}  // namespace merenja

const char* const kUlaz = R"(# konfiguracija
kanal temp min=-20 max=60 jedinica=C
kanal vlaga min=0 max=100 jedinica=%
kanal pritisak min=900 max=1100 jedinica=hPa
kanal struja min=0 max=10 jedinica=A
kanal los min=5

# merenja
temp 21.5
vlaga 40
temp 85
pritisak 1013.2
vlaga abc
napon 12
temp 22
pritisak
vlaga 55
temp 21.8
)";

int main() {
    std::istringstream ulaz(kUlaz);
    const merenja::Podaci p = merenja::ucitaj(ulaz);
    merenja::ispisiIzvestaj(std::cout, p);
}

/* EXPECTED OUTPUT
kanala: 4 (neispravnih redova konfiguracije: 1)
merenja: 6 ispravnih od 10
greške: nepoznat kanal 1, nije broj 1, van opsega 1, loš format 1

kanal       n       min       max    prosek
temp        3      21.5      22.0      21.8 C
vlaga       2      40.0      55.0      47.5 %
pritisak    1    1013.2    1013.2    1013.2 hPa
struja      0         -         -         - A
*/
