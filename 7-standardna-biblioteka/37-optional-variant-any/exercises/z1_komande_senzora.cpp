// VRSTA: upotreba
//
// Zadatak 1 -- optional za "možda uspe", variant za komande, visit za
// izvršavanje (sekcije 1, 4, 5)
//   ./build.sh 7-standardna-biblioteka/37-optional-variant-any/exercises/z1_komande_senzora.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_komande_senzora.cpp
//
// Korak 1: std::optional<double> procitajBroj(const std::string& s)
//   -- std::istringstream, >> u double; nullopt ako čitanje ne uspe ILI
//   posle broja ostane još nešto ("2x").
// Korak 2: std::optional<Komanda> parsirajKomandu(const std::string& red)
//   -- "reset" -> Reset{}; "get kanal" -> Procitaj{kanal};
//   "set kanal broj" -> Postavi{kanal, broj} (broj preko procitajBroj);
//   sve ostalo -> nullopt.
// Korak 3: std::string izvrsi(std::map<std::string, double>& stanje, const Komanda& k)
//   -- std::visit sa Preopterecen{...}: Postavi upiše i vrati "ok";
//   Procitaj vrati "kanal = vrednost" ili "kanal: nema"; Reset obriše sve
//   i vrati "obrisano". (Šta se desi ako dodaš četvrtu komandu u
//   Komanda, a ovde je zaboraviš?)

#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

struct Postavi {
    std::string kanal;
    double vrednost;
};
struct Procitaj {
    std::string kanal;
};
struct Reset {};
using Komanda = std::variant<Postavi, Procitaj, Reset>;

template <typename... F>
struct Preopterecen : F... {
    using F::operator()...;
};
template <typename... F>
Preopterecen(F...) -> Preopterecen<F...>;

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // for (const char* s : {"21.5", "2x", ""}) {
    //     auto v = procitajBroj(s);
    //     std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v).substr(0, 4) : "nije broj") << '\n';
    // }

    // Korak 2 i 3 -- otkomentariši:
    // std::map<std::string, double> stanje;
    // std::vector<std::string> redovi{"set temp 21.5", "get temp", "get vlaga", "set vlaga x", "reset", "get temp", "skok"};
    // for (const auto& red : redovi) {
    //     std::cout << red << " => ";
    //     if (auto k = parsirajKomandu(red))
    //         std::cout << izvrsi(stanje, *k) << '\n';
    //     else
    //         std::cout << "neispravna komanda\n";
    // }
}

/* OČEKIVANI IZLAZ
"21.5" -> 21.5
"2x" -> nije broj
"" -> nije broj
set temp 21.5 => ok
get temp => temp = 21.5
get vlaga => vlaga: nema
set vlaga x => neispravna komanda
reset => obrisano
get temp => temp: nema
skok => neispravna komanda
*/
