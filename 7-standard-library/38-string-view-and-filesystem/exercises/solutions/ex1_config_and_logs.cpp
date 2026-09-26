// Rešenje zadatka ex1_config_and_logs.

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
std::optional<int> toNumber(std::string_view s) {
    int v = 0;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc() || ptr != s.data() + s.size()) return std::nullopt;
    return v;
}

// Korak 2: pogledi za sečenje, ali KLJUČ u mapi je std::string -- mapa
// živi duže od teksta koji je parsiran.
std::map<std::string, int> parseConfig(std::string_view text) {
    std::map<std::string, int> m;
    while (!text.empty()) {
        auto end = text.find(';');
        std::string_view pair = text.substr(0, end);
        text.remove_prefix(end == std::string_view::npos ? text.size() : end + 1);
        auto equals = pair.find('=');
        if (equals == std::string_view::npos) continue;
        if (auto v = toNumber(pair.substr(equals + 1))) m[std::string(pair.substr(0, equals))] = *v;
    }
    return m;
}

// Korak 3: samo obični fajlovi sa .log; sortirano jer redosled nije određen.
std::vector<std::pair<std::string, std::uintmax_t>> logFiles(const fs::path& dir) {
    std::vector<std::pair<std::string, std::uintmax_t>> r;
    for (const auto& e : fs::directory_iterator(dir))
        if (e.is_regular_file() && e.path().extension() == ".log")
            r.emplace_back(e.path().filename().string(), e.file_size());
    std::sort(r.begin(), r.end());
    return r;
}

int main() {
    for (std::string_view s : {"42", "-7", "4x", ""}) {
        auto v = toNumber(s);
        std::cout << '"' << s << "\" -> " << (v ? std::to_string(*v) : "not a number") << '\n';
    }

    std::string text = "temp=21;humidity=40;pressure=x;no_value;voltage=230";
    for (const auto& [k, v] : parseConfig(text)) std::cout << k << " = " << v << '\n';

    fs::path dir = fs::temp_directory_path() /
                   ("mcpp_s23_z1_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(dir / "old");
    std::ofstream(dir / "hall.log") << "21\n22\n";
    std::ofstream(dir / "boiler.log") << "80\n";
    std::ofstream(dir / "note.txt") << "x";
    std::uintmax_t total = 0;
    for (const auto& [name, size] : logFiles(dir)) {
        std::cout << name << ": " << size << " B\n";
        total += size;
    }
    std::cout << "total: " << total << " B\n";
    fs::remove_all(dir);
}
