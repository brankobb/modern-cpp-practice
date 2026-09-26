// Rešenje zadatka ex1_reading_analysis.

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
std::map<std::string, std::vector<double>> load(const std::vector<std::string>& log) {
    std::map<std::string, std::vector<double>> m;
    for (const auto& line : log) {
        std::istringstream in(line);
        std::string sensor;
        double v = 0;
        if (in >> sensor >> v) m[sensor].push_back(v);
    }
    return m;
}

struct Stats {
    double min, max, mean, median;
};

// Korak 2: minmax_element jednim prolazom; accumulate sa 0.0 (ne 0!);
// nth_element na kopiji -- O(n), bez sortiranja celog niza.
Stats computeStats(std::vector<double> v) {   // kopija: nth_element menja redosled
    auto [mn, mx] = std::minmax_element(v.begin(), v.end());
    Stats s{*mn, *mx, std::accumulate(v.begin(), v.end(), 0.0) / static_cast<double>(v.size()), 0};
    auto middle = v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2);
    std::nth_element(v.begin(), middle, v.end());
    s.median = *middle;
    return s;
}

// Korak 3: sve vrednosti u jedan vektor, partial_sort samo prvih n.
std::vector<double> largestN(const std::map<std::string, std::vector<double>>& m, std::size_t n) {
    std::vector<double> all;
    for (const auto& [name, v] : m) all.insert(all.end(), v.begin(), v.end());
    n = std::min(n, all.size());
    std::partial_sort(all.begin(), all.begin() + static_cast<std::ptrdiff_t>(n), all.end(), std::greater<>{});
    all.resize(n);
    return all;
}

// Korak 4: copy_if + back_inserter -- cilj raste sam.
std::vector<double> above(const std::vector<double>& v, double threshold) {
    std::vector<double> r;
    std::copy_if(v.begin(), v.end(), std::back_inserter(r), [threshold](double x) { return x > threshold; });
    return r;
}

int main() {
    std::vector<std::string> log{
        "temp 21.5", "humidity 40",   "temp 22.0", "temp 35.5", "humidity 42",
        "temp 21.0", "humidity 90",   "temp 22.5", "humidity 44",  "invalid",
    };
    auto m = load(log);
    std::cout << std::fixed << std::setprecision(1);
    for (const auto& [name, v] : m) std::cout << name << ": " << v.size() << " readings\n";

    for (const auto& [name, v] : m) {
        Stats s = computeStats(v);
        std::cout << name << ": min " << s.min << ", max " << s.max << ", mean " << s.mean
                  << ", median " << s.median << '\n';
    }

    std::cout << "3 largest values:";
    for (double x : largestN(m, 3)) std::cout << ' ' << x;
    std::cout << '\n';

    std::cout << "temp above 30:";
    for (double x : above(m.at("temp"), 30)) std::cout << ' ' << x;
    std::cout << "\nhumidity above 80: " << above(m.at("humidity"), 80).size() << '\n';
}
