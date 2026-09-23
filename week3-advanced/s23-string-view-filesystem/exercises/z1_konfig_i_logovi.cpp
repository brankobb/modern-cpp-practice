// VRSTA: upotreba
//
// Zadatak 1 -- parsiranje bez kopija preko string_view i from_chars,
// pregled log fajlova preko filesystem-a (sekcije 1, 2, 5, 6)
//   ./build.sh week3-advanced/s23-string-view-filesystem/exercises/z1_konfig_i_logovi.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_konfig_i_logovi.cpp
//
// Korak 1: std::optional<int> uBroj(std::string_view s)
//   -- std::from_chars (<charconv>, C++17): vraća {ptr, ec}. Broj je
//   ispravan samo ako ec == std::errc() I ptr == s.data() + s.size()
//   ("4x" nije broj). Zašto ne std::stoi? (Pogledaj šta prima.)
// Korak 2: std::map<std::string, int> parsirajKonfig(std::string_view tekst)
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
    //     auto v = uBroj(s);
    //     std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v) : "nije broj") << '\n';
    // }

    // Korak 2 -- otkomentariši:
    // std::string tekst = "temp=21;vlaga=40;pritisak=x;bez_vrednosti;napon=230";
    // for (const auto& [k, v] : parsirajKonfig(tekst)) std::cout << k << " = " << v << '\n';

    // Korak 3 -- otkomentariši:
    // fs::path dir = fs::temp_directory_path() /
    //                ("mcpp_s23_z1_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    // fs::create_directories(dir / "stari");
    // std::ofstream(dir / "hala.log") << "21\n22\n";
    // std::ofstream(dir / "kotao.log") << "80\n";
    // std::ofstream(dir / "napomena.txt") << "x";
    // std::uintmax_t ukupno = 0;
    // for (const auto& [ime, velicina] : logovi(dir)) {
    //     std::cout << ime << ": " << velicina << " B\n";
    //     ukupno += velicina;
    // }
    // std::cout << "ukupno: " << ukupno << " B\n";
    // fs::remove_all(dir);
}

/* OČEKIVANI IZLAZ
"42" -> 42
"-7" -> -7
"4x" -> nije broj
"" -> nije broj
napon = 230
temp = 21
vlaga = 40
hala.log: 6 B
kotao.log: 3 B
ukupno: 9 B
*/
