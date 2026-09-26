// Završna vežba dela 7 -- indeks log fajlova
//   ./build.sh 7-standard-library/capstone/task.cpp
// Uputstvo i spisak lekcija: 7-standard-library/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a. napraviLogove() (dato, na dnu) pravi
// privremeni direktorijum sa tri .log fajla i jednim .txt; main ga na
// kraju briše.
//
// Korak 1: std::vector<fs::path> nadjiLogove(const fs::path& koren)
//   -- svi obični fajlovi sa ekstenzijom .log, i u poddirektorijumima;
//   sortirani, jer redosled obilaska nije određen (lekcija 38, sekcije 5 i 6).
// Korak 2: parsiranje reda "08:15 temp 22.0" bez kopiranja teksta
//   -- std::string_view sledecaRec(std::string_view& s): sledeća reč,
//   i skrati pogled (lekcija 38, sekcije 1 i 2).
//   -- std::optional<double> uBroj(std::string_view): std::from_chars; ceo
//   pogled mora da bude broj (lekcija 37, sekcija 1).
//   -- Red parsiraj(std::string_view red): Zapis ili Greska ("loš format"
//   ako nisu tačno tri reči, "nije broj"); Red je std::variant (lekcija 37,
//   sekcije 4 i 5).
// Korak 3: Indeks napraviIndeks(const fs::path& koren)
//   -- čitaj sve logove red po red (std::ifstream, std::getline); prazne
//   redove i komentare ('#') preskoči; std::visit sa Preopterecen:
//   zapis ide u poKanalu i kanaliPoFajlu, greška u greske
//   (lekcija 35, sekcije 1, 3 i 4).
// Korak 4: izveštaj preko algoritama (lekcija 36, sekcije 2, 3 i 4)
//   -- std::optional<double> prag(kanal): temp 80, pritisak 2, ostali nemaju.
//   -- double medijana(std::vector<double>): std::nth_element; za paran
//   broj elemenata uzima gornji od dva srednja.
//   -- izvestaj(ix): po kanalu n, min i max (std::minmax_element),
//   medijana i broj iznad praga (std::count_if); greške sortirane (ne
//   iteriraj unordered_map direktno za ispis); 3 najviše temperature
//   (std::partial_sort); kanali prisutni u SVIM fajlovima
//   (std::set_intersection, fajl po fajl).

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace fs = std::filesystem;

template <typename... F>
struct Preopterecen : F... {
    using F::operator()...;
};
template <typename... F>
Preopterecen(F...) -> Preopterecen<F...>;

struct Zapis {
    std::string vreme, kanal;
    double vrednost;
};
struct Greska {
    std::string razlog;
};
using Red = std::variant<Zapis, Greska>;

struct Indeks {
    std::map<std::string, std::vector<double>> poKanalu;
    std::map<std::string, std::set<std::string>> kanaliPoFajlu;   // ključ: putanja relativno od korena
    std::unordered_map<std::string, int> greske;                   // razlog -> koliko puta
    int zapisa = 0;
};

// TODO korak 1, 2, 3, 4

// ---------------------------------------------------------------- podaci
fs::path napraviLogove() {
    const auto broj = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path koren = fs::temp_directory_path() / ("mcpp_zv7_" + std::to_string(broj));
    fs::create_directories(koren / "arhiva");
    std::ofstream(koren / "hala.log") << "08:00 temp 21.5\n08:00 vlaga 40\n08:15 temp 22.0\n# ručno očitano\n"
                                         "08:30 temp abc\n08:30 vlaga 42\n";
    std::ofstream(koren / "kotao.log") << "08:00 temp 78.0\n08:05 pritisak 1.8\n08:10 temp 83.5\n08:15 temp 81.0\n"
                                          "08:20 pritisak 2.4\n08:25\n";
    std::ofstream(koren / "arhiva" / "2024-01.log") << "07:00 temp 19.0\n07:30 vlaga 55 %\n07:45 temp 18.5\n";
    std::ofstream(koren / "napomena.txt") << "nije log\n";
    return koren;
}

int main() {
    const fs::path koren = napraviLogove();

    // Korak 1 -- otkomentariši:
    // std::cout << "== korak 1: pronalaženje logova\n";
    // for (const auto& p : nadjiLogove(koren)) std::cout << "  " << p.lexically_relative(koren).generic_string() << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "== korak 2: parsiranje reda\n";
    // for (const char* red : {"08:15 temp 22.0", "08:30 temp abc", "08:25", "07:30 vlaga 55 %"}) {
    //     std::cout << "  \"" << red << "\" -> ";
    //     std::visit(Preopterecen{
    //                    [](const Zapis& z) { std::cout << z.kanal << " = " << z.vrednost << " u " << z.vreme << '\n'; },
    //                    [](const Greska& g) { std::cout << "greška: " << g.razlog << '\n'; },
    //                },
    //                parsiraj(red));
    // }

    // Korak 3 -- otkomentariši:
    // std::cout << "== korak 3: indeks\n";
    // const Indeks ix = napraviIndeks(koren);
    // for (const auto& [fajl, kanali] : ix.kanaliPoFajlu) {
    //     std::cout << "  " << fajl << ':';
    //     for (const auto& kanal : kanali) std::cout << ' ' << kanal;
    //     std::cout << '\n';
    // }

    // Korak 4 -- otkomentariši:
    // std::cout << "== korak 4: izveštaj\n";
    // izvestaj(ix);

    fs::remove_all(koren);   // uvek: briše privremeni direktorijum
}

/* EXPECTED OUTPUT
== korak 1: pronalaženje logova
  arhiva/2024-01.log
  hala.log
  kotao.log
== korak 2: parsiranje reda
  "08:15 temp 22.0" -> temp = 22 u 08:15
  "08:30 temp abc" -> greška: nije broj
  "08:25" -> greška: loš format
  "07:30 vlaga 55 %" -> greška: loš format
== korak 3: indeks
  arhiva/2024-01.log: temp
  hala.log: temp vlaga
  kotao.log: pritisak temp
== korak 4: izveštaj
zapisa: 11
  pritisak  n=2 min=1.8 max=2.4 medijana=2.4 iznad 2.0: 1
  temp      n=7 min=18.5 max=83.5 medijana=22.0 iznad 80.0: 2
  vlaga     n=2 min=40.0 max=42.0 medijana=42.0
greške: loš format=2 nije broj=1
3 najviše temp: 83.5 81.0 78.0
u svim fajlovima: temp
*/
