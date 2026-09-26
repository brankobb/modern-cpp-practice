// KIND: usage
//
// Zadatak 1 -- parsiranje bez kopija preko string_view i from_chars,
// pregled log fajlova preko filesystem-a (sekcije 1, 2, 5, 6)
//   ./build.sh 7-standard-library/38-string-view-and-filesystem/exercises/ex1_config_and_logs.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_config_and_logs.cpp
//
// Korak 1: std::optional<int> toNumber(std::string_view s)
//   -- std::from_chars (<charconv>, C++17): vraća {ptr, ec}. Broj je
//   ispravan samo ako ec == std::errc() I ptr == s.data() + s.size()
//   ("4x" nije broj). Zašto ne std::stoi? (Pogledaj šta prima.)
// Korak 2: std::map<std::string, int> parseConfig(std::string_view text)
//   -- "k=v;k=v;..." seci pogledima (find, substr, remove_prefix); par bez
//   '=' ili sa vrednošću koja nije broj preskoči. Zašto ključ u mapi mora
//   biti std::string, a ne string_view?
// Korak 3: std::vector<std::pair<std::string, std::uintmax_t>> logovi(const fs::path& dir)
//   -- directory_iterator; samo is_regular_file() sa extension() == ".log";
//   par (ime fajla, file_size); sortiraj.

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // for (std::string_view s : {"42", "-7", "4x", ""}) {
    //     auto v = toNumber(s);
    //     std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v) : "not a number") << '\n';
    // }

    // Korak 2 -- otkomentariši:
    // std::string text = "temp=21;humidity=40;pressure=x;no_value;voltage=230";
    // for (const auto& [k, v] : parseConfig(text)) std::cout << k << " = " << v << '\n';

    // Korak 3 -- otkomentariši:
    // fs::path dir = fs::temp_directory_path() /
    //                ("mcpp_s23_z1_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    // fs::create_directories(dir / "old");
    // std::ofstream(dir / "hall.log") << "21\n22\n";
    // std::ofstream(dir / "boiler.log") << "80\n";
    // std::ofstream(dir / "note.txt") << "x";
    // std::uintmax_t total = 0;
    // for (const auto& [name, size] : logFiles(dir)) {
    //     std::cout << name << ": " << size << " B\n";
    //     total += size;
    // }
    // std::cout << "total: " << total << " B\n";
    // fs::remove_all(dir);
}

/* EXPECTED OUTPUT
"42" -> 42
"-7" -> -7
"4x" -> not a number
"" -> not a number
humidity = 40
temp = 21
voltage = 230
boiler.log: 3 B
hall.log: 6 B
total: 9 B
*/
