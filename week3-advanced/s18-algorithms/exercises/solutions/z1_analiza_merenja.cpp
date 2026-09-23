// Rešenje zadatka z1_analiza_merenja.

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// Korak 1: >> čita reč pa broj; map drži senzore sortirane po imenu.
std::map<std::string, std::vector<double>> ucitaj(const std::vector<std::string>& log) {
    std::map<std::string, std::vector<double>> m;
    for (const auto& red : log) {
        std::istringstream in(red);
        std::string senzor;
        double v = 0;
        if (in >> senzor >> v) m[senzor].push_back(v);
    }
    return m;
}

struct Statistika {
    double min, max, prosek, medijana;
};

// Korak 2: minmax_element jednim prolazom; accumulate sa 0.0 (ne 0!);
// nth_element na kopiji -- O(n), bez sortiranja celog niza.
Statistika statistika(std::vector<double> v) {   // kopija: nth_element menja redosled
    auto [mn, mx] = std::minmax_element(v.begin(), v.end());
    Statistika s{*mn, *mx, std::accumulate(v.begin(), v.end(), 0.0) / static_cast<double>(v.size()), 0};
    auto sredina = v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2);
    std::nth_element(v.begin(), sredina, v.end());
    s.medijana = *sredina;
    return s;
}

// Korak 3: sve vrednosti u jedan vektor, partial_sort samo prvih n.
std::vector<double> najvecih(const std::map<std::string, std::vector<double>>& m, std::size_t n) {
    std::vector<double> sve;
    for (const auto& [ime, v] : m) sve.insert(sve.end(), v.begin(), v.end());
    n = std::min(n, sve.size());
    std::partial_sort(sve.begin(), sve.begin() + static_cast<std::ptrdiff_t>(n), sve.end(), std::greater<>{});
    sve.resize(n);
    return sve;
}

// Korak 4: copy_if + back_inserter -- cilj raste sam.
std::vector<double> iznad(const std::vector<double>& v, double prag) {
    std::vector<double> r;
    std::copy_if(v.begin(), v.end(), std::back_inserter(r), [prag](double x) { return x > prag; });
    return r;
}

int main() {
    std::vector<std::string> log{
        "temp 21.5", "vlaga 40",   "temp 22.0", "temp 35.5", "vlaga 42",
        "temp 21.0", "vlaga 90",   "temp 22.5", "vlaga 44",  "neispravno",
    };
    auto m = ucitaj(log);
    std::cout << std::fixed << std::setprecision(1);
    for (const auto& [ime, v] : m) std::cout << ime << ": " << v.size() << " merenja\n";

    for (const auto& [ime, v] : m) {
        Statistika s = statistika(v);
        std::cout << ime << ": min " << s.min << ", max " << s.max << ", prosek " << s.prosek
                  << ", medijana " << s.medijana << '\n';
    }

    std::cout << "3 najveće vrednosti:";
    for (double x : najvecih(m, 3)) std::cout << ' ' << x;
    std::cout << '\n';

    std::cout << "temp iznad 30:";
    for (double x : iznad(m.at("temp"), 30)) std::cout << ' ' << x;
    std::cout << "\nvlaga iznad 80: " << iznad(m.at("vlaga"), 80).size() << '\n';
}
