// Rešenje zadatka ex1_konfig_i_logovi.

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

// Korak 1: from_chars ne alocira, ne baca i ne zavisi od locale-a; ptr
// kaže dokle je stigao -- ceo pogled mora biti potrošen. std::stoi prima
// const std::string& (kopija pogleda, alokacija), baca izuzetak za "x" i
// ćutke prihvati "4x" kao 4.
std::optional<int> uBroj(std::string_view s) {
    int v = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc() || ptr != s.data() + s.size()) return std::nullopt;
    return v;
}

// Korak 2: pogledi za sečenje, ali KLJUČ u mapi je std::string -- mapa
// živi duže od teksta koji je parsiran.
std::map<std::string, int> parsirajKonfig(std::string_view tekst) {
    std::map<std::string, int> m;
    while (!tekst.empty()) {
        auto kraj = tekst.find(';');
        std::string_view par = tekst.substr(0, kraj);
        tekst.remove_prefix(kraj == std::string_view::npos ? tekst.size() : kraj + 1);
        auto jednako = par.find('=');
        if (jednako == std::string_view::npos) continue;
        if (auto v = uBroj(par.substr(jednako + 1))) m[std::string(par.substr(0, jednako))] = *v;
    }
    return m;
}

// Korak 3: samo obični fajlovi sa .log; sortirano jer redosled nije određen.
std::vector<std::pair<std::string, std::uintmax_t>> logovi(const fs::path& dir) {
    std::vector<std::pair<std::string, std::uintmax_t>> r;
    for (const auto& e : fs::directory_iterator(dir))
        if (e.is_regular_file() && e.path().extension() == ".log")
            r.emplace_back(e.path().filename().string(), e.file_size());
    std::sort(r.begin(), r.end());
    return r;
}

int main() {
    for (std::string_view s : {"42", "-7", "4x", ""}) {
        auto v = uBroj(s);
        std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v) : "nije broj") << '\n';
    }

    std::string tekst = "temp=21;vlaga=40;pritisak=x;bez_vrednosti;napon=230";
    for (const auto& [k, v] : parsirajKonfig(tekst)) std::cout << k << " = " << v << '\n';

    fs::path dir = fs::temp_directory_path() /
                   ("mcpp_s23_z1_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(dir / "stari");
    std::ofstream(dir / "hala.log") << "21\n22\n";
    std::ofstream(dir / "kotao.log") << "80\n";
    std::ofstream(dir / "napomena.txt") << "x";
    std::uintmax_t ukupno = 0;
    for (const auto& [ime, velicina] : logovi(dir)) {
        std::cout << ime << ": " << velicina << " B\n";
        ukupno += velicina;
    }
    std::cout << "ukupno: " << ukupno << " B\n";
    fs::remove_all(dir);
}
