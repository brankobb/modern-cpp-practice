// Završna vežba dela 7 -- indeks log fajlova
//   ./build.sh 7-standard-library/capstone/task.cpp
// Uputstvo i spisak lekcija: 7-standard-library/capstone/notes.md
// Rešenje: solution.cpp (otvori tek kad tvoj izlaz bude isti kao blok
// EXPECTED OUTPUT na dnu fajla).
//
// Fajl se kompajlira i ovakav. Piši redom; posle svakog koraka
// otkomentariši njegov deo main()-a. makeLogs() (dato, na dnu) pravi
// privremeni direktorijum sa tri .log fajla i jednim .txt; main ga na
// kraju briše.
//
// Korak 1: std::vector<fs::path> findLogs(const fs::path& root)
//   -- svi obični fajlovi sa ekstenzijom .log, i u poddirektorijumima;
//   sortirani, jer redosled obilaska nije određen (lekcija 38, sekcije 5 i 6).
// Korak 2: parsiranje reda "08:15 temp 22.0" bez kopiranja teksta
//   -- std::string_view nextWord(std::string_view& s): sledeća reč,
//   i skrati pogled (lekcija 38, sekcije 1 i 2).
//   -- std::optional<double> toNumber(std::string_view): std::from_chars; ceo
//   pogled mora da bude broj (lekcija 37, sekcija 1).
//   -- Line parse(std::string_view line): Record ili Error ("bad format"
//   ako nisu tačno tri reči, "not a number"); Line je std::variant (lekcija 37,
//   sekcije 4 i 5).
// Korak 3: Index buildIndex(const fs::path& root)
//   -- čitaj sve logove red po red (std::ifstream, std::getline); prazne
//   redove i komentare ('#') preskoči; std::visit sa Overloaded:
//   zapis ide u byChannel i channelsByFile, greška u errors
//   (lekcija 35, sekcije 1, 3 i 4).
// Korak 4: izveštaj preko algoritama (lekcija 36, sekcije 2, 3 i 4)
//   -- std::optional<double> threshold(channel): temp 80, pressure 2, ostali nemaju.
//   -- double median(std::vector<double>): std::nth_element; za paran
//   broj elemenata uzima gornji od dva srednja.
//   -- report(ix): po kanalu n, min i max (std::minmax_element),
//   median i broj iznad praga (std::count_if); greške sortirane (ne
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
struct Overloaded : F... {
    using F::operator()...;
};
template <typename... F>
Overloaded(F...) -> Overloaded<F...>;

struct Record {
    std::string time, channel;
    double value;
};
struct Error {
    std::string reason;
};
using Line = std::variant<Record, Error>;

struct Index {
    std::map<std::string, std::vector<double>> byChannel;
    std::map<std::string, std::set<std::string>> channelsByFile;   // ključ: putanja relativno od korena
    std::unordered_map<std::string, int> errors;                   // razlog -> koliko puta
    int records = 0;
};

// TODO korak 1, 2, 3, 4

// ---------------------------------------------------------------- podaci
fs::path makeLogs() {
    const auto number = std::chrono::steady_clock::now().time_since_epoch().count();
    const fs::path root = fs::temp_directory_path() / ("mcpp_zv7_" + std::to_string(number));
    fs::create_directories(root / "archive");
    std::ofstream(root / "hall.log") << "08:00 temp 21.5\n08:00 humidity 40\n08:15 temp 22.0\n# read manually\n"
                                         "08:30 temp abc\n08:30 humidity 42\n";
    std::ofstream(root / "boiler.log") << "08:00 temp 78.0\n08:05 pressure 1.8\n08:10 temp 83.5\n08:15 temp 81.0\n"
                                          "08:20 pressure 2.4\n08:25\n";
    std::ofstream(root / "archive" / "2024-01.log") << "07:00 temp 19.0\n07:30 humidity 55 %\n07:45 temp 18.5\n";
    std::ofstream(root / "note.txt") << "not a log\n";
    return root;
}

int main() {
    const fs::path root = makeLogs();

    // Korak 1 -- otkomentariši:
    // std::cout << "== step 1: finding logs\n";
    // for (const auto& p : findLogs(root)) std::cout << "  " << p.lexically_relative(root).generic_string() << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "== step 2: parsing a line\n";
    // for (const char* line : {"08:15 temp 22.0", "08:30 temp abc", "08:25", "07:30 humidity 55 %"}) {
    //     std::cout << "  \"" << line << "\" -> ";
    //     std::visit(Overloaded{
    //                    [](const Record& z) { std::cout << z.channel << " = " << z.value << " at " << z.time << '\n'; },
    //                    [](const Error& g) { std::cout << "error: " << g.reason << '\n'; },
    //                },
    //                parse(line));
    // }

    // Korak 3 -- otkomentariši:
    // std::cout << "== step 3: index\n";
    // const Index ix = buildIndex(root);
    // for (const auto& [file, channels] : ix.channelsByFile) {
    //     std::cout << "  " << file << ':';
    //     for (const auto& channel : channels) std::cout << ' ' << channel;
    //     std::cout << '\n';
    // }

    // Korak 4 -- otkomentariši:
    // std::cout << "== step 4: report\n";
    // report(ix);

    fs::remove_all(root);   // uvek: briše privremeni direktorijum
}

/* EXPECTED OUTPUT
== step 1: finding logs
  archive/2024-01.log
  boiler.log
  hall.log
== step 2: parsing a line
  "08:15 temp 22.0" -> temp = 22 at 08:15
  "08:30 temp abc" -> error: not a number
  "08:25" -> error: bad format
  "07:30 humidity 55 %" -> error: bad format
== step 3: index
  archive/2024-01.log: temp
  boiler.log: pressure temp
  hall.log: humidity temp
== step 4: report
records: 11
  humidity  n=2 min=40.0 max=42.0 median=42.0
  pressure  n=2 min=1.8 max=2.4 median=2.4 above 2.0: 1
  temp      n=7 min=18.5 max=83.5 median=22.0 above 80.0: 2
errors: bad format=2 not a number=1
top 3 temp: 83.5 81.0 78.0
in all files: temp
*/
