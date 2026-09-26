// Rešenje završne vežbe dela 7: indeks log fajlova.

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

// ---------------------------------------------------------------- korak 1
// Svi .log fajlovi ispod korena, rekurzivno, sortirani (redosled obilaska
// nije određen).
std::vector<fs::path> nadjiLogove(const fs::path& koren) {
    std::vector<fs::path> logovi;
    for (const auto& e : fs::recursive_directory_iterator(koren))
        if (e.is_regular_file() && e.path().extension() == ".log") logovi.push_back(e.path());
    std::sort(logovi.begin(), logovi.end());
    return logovi;
}

// ---------------------------------------------------------------- korak 2
struct Zapis {
    std::string vreme, kanal;
    double vrednost;
};
struct Greska {
    std::string razlog;
};
using Red = std::variant<Zapis, Greska>;

// Sledeća reč iz pogleda (razmaci se preskaču); pogled se skraćuje.
std::string_view sledecaRec(std::string_view& s) {
    const auto pocetak = s.find_first_not_of(' ');
    if (pocetak == std::string_view::npos) {
        s = {};
        return {};
    }
    s.remove_prefix(pocetak);
    const auto kraj = std::min(s.find(' '), s.size());
    const std::string_view rec = s.substr(0, kraj);
    s.remove_prefix(kraj);
    return rec;
}

std::optional<double> uBroj(std::string_view s) {
    double v = 0;
    const auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), v);
    if (ec != std::errc() || ptr != s.data() + s.size()) return std::nullopt;
    return v;
}

// "08:15 temp 22.0"
Red parsiraj(std::string_view red) {
    const auto vreme = sledecaRec(red);
    const auto kanal = sledecaRec(red);
    const auto broj = sledecaRec(red);
    if (broj.empty() || !sledecaRec(red).empty()) return Greska{"loš format"};
    const auto v = uBroj(broj);
    if (!v) return Greska{"nije broj"};
    return Zapis{std::string(vreme), std::string(kanal), *v};
}

// ---------------------------------------------------------------- korak 3
struct Indeks {
    std::map<std::string, std::vector<double>> poKanalu;
    std::map<std::string, std::set<std::string>> kanaliPoFajlu;   // ključ: putanja relativno od korena
    std::unordered_map<std::string, int> greske;                   // razlog -> koliko puta
    int zapisa = 0;
};

Indeks napraviIndeks(const fs::path& koren) {
    Indeks ix;
    for (const fs::path& p : nadjiLogove(koren)) {
        const std::string ime = p.lexically_relative(koren).generic_string();
        std::set<std::string>& kanali = ix.kanaliPoFajlu[ime];
        std::ifstream in(p);
        std::string red;
        while (std::getline(in, red)) {
            if (red.empty() || red[0] == '#') continue;
            std::visit(Preopterecen{
                           [&](const Zapis& z) {
                               ix.poKanalu[z.kanal].push_back(z.vrednost);
                               kanali.insert(z.kanal);
                               ++ix.zapisa;
                           },
                           [&](const Greska& g) { ++ix.greske[g.razlog]; },
                       },
                       parsiraj(red));
        }
    }
    return ix;
}

// ---------------------------------------------------------------- korak 4
std::optional<double> prag(const std::string& kanal) {
    static const std::map<std::string, double> pragovi{{"temp", 80.0}, {"pritisak", 2.0}};
    const auto it = pragovi.find(kanal);
    if (it == pragovi.end()) return std::nullopt;
    return it->second;
}

double medijana(std::vector<double> v) {   // kopija: nth_element menja redosled
    const auto sredina = v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2);
    std::nth_element(v.begin(), sredina, v.end());
    return *sredina;
}

void izvestaj(const Indeks& ix) {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "zapisa: " << ix.zapisa << '\n';
    for (const auto& [kanal, v] : ix.poKanalu) {
        const auto [mn, mx] = std::minmax_element(v.begin(), v.end());
        std::cout << "  " << std::left << std::setw(9) << kanal << std::right << " n=" << v.size() << " min=" << *mn
                  << " max=" << *mx << " medijana=" << medijana(v);
        if (const auto p = prag(kanal))
            std::cout << " iznad " << *p << ": "
                      << std::count_if(v.begin(), v.end(), [p](double x) { return x > *p; });
        std::cout << '\n';
    }

    // unordered_map nema redosled: za ispis sortiraj.
    std::vector<std::pair<std::string, int>> greske(ix.greske.begin(), ix.greske.end());
    std::sort(greske.begin(), greske.end());
    std::cout << "greške:";
    for (const auto& [razlog, n] : greske) std::cout << ' ' << razlog << '=' << n;
    std::cout << '\n';

    std::vector<double> temp = ix.poKanalu.at("temp");
    const auto k = std::min<std::size_t>(3, temp.size());
    std::partial_sort(temp.begin(), temp.begin() + static_cast<std::ptrdiff_t>(k), temp.end(), std::greater<>{});
    std::cout << "3 najviše temp:";
    for (std::size_t i = 0; i < k; ++i) std::cout << ' ' << temp[i];
    std::cout << '\n';

    // Kanali koji postoje u SVIM fajlovima: presek skupova, jedan po jedan.
    auto it = ix.kanaliPoFajlu.begin();
    std::set<std::string> zajednicki = it->second;
    for (++it; it != ix.kanaliPoFajlu.end(); ++it) {
        std::set<std::string> presek;
        std::set_intersection(zajednicki.begin(), zajednicki.end(), it->second.begin(), it->second.end(),
                              std::inserter(presek, presek.end()));
        zajednicki = std::move(presek);
    }
    std::cout << "u svim fajlovima:";
    for (const auto& kanal : zajednicki) std::cout << ' ' << kanal;
    std::cout << '\n';
}

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

    std::cout << "== korak 1: pronalaženje logova\n";
    for (const auto& p : nadjiLogove(koren)) std::cout << "  " << p.lexically_relative(koren).generic_string() << '\n';

    std::cout << "== korak 2: parsiranje reda\n";
    for (const char* red : {"08:15 temp 22.0", "08:30 temp abc", "08:25", "07:30 vlaga 55 %"}) {
        std::cout << "  \"" << red << "\" -> ";
        std::visit(Preopterecen{
                       [](const Zapis& z) { std::cout << z.kanal << " = " << z.vrednost << " u " << z.vreme << '\n'; },
                       [](const Greska& g) { std::cout << "greška: " << g.razlog << '\n'; },
                   },
                   parsiraj(red));
    }

    std::cout << "== korak 3: indeks\n";
    const Indeks ix = napraviIndeks(koren);
    for (const auto& [fajl, kanali] : ix.kanaliPoFajlu) {
        std::cout << "  " << fajl << ':';
        for (const auto& kanal : kanali) std::cout << ' ' << kanal;
        std::cout << '\n';
    }

    std::cout << "== korak 4: izveštaj\n";
    izvestaj(ix);

    fs::remove_all(koren);   // uvek: briše privremeni direktorijum
}
