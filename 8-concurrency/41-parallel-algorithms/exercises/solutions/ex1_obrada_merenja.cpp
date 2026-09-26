// Rešenje zadatka ex1_obrada_merenja.

#include <algorithm>
#include <execution>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace ex = std::execution;

// Korak 1: svaki element u svoje mesto -- nema deljenja, pa nema ni race-a.
std::vector<double> kalibrisi(const std::vector<int>& sirovo, double k, double n) {
    std::vector<double> r(sirovo.size());
    std::transform(ex::par, sirovo.begin(), sirovo.end(), r.begin(), [k, n](int x) { return k * x + n; });
    return r;
}

// Korak 2: reduce i transform_reduce sami skupljaju; + je asocijativno.
// (Vrednosti su polovine celih brojeva, pa su zbirovi u double tačni i
// ne zavise od redosleda sabiranja -- inače bi par mogao da se razlikuje
// od seq u poslednjim ciframa.)
struct Statistika {
    double prosek, varijansa;
    long iznadPraga;
};

Statistika statistika(const std::vector<double>& v, double prag) {
    double n = static_cast<double>(v.size());
    double prosek = std::reduce(ex::par, v.begin(), v.end(), 0.0) / n;
    double varijansa = std::transform_reduce(ex::par, v.begin(), v.end(), 0.0, std::plus<>{},
                                             [prosek](double x) { return (x - prosek) * (x - prosek); }) / n;
    long iznad = std::count_if(ex::par, v.begin(), v.end(), [prag](double x) { return x > prag; });
    return {prosek, varijansa, iznad};
}

// Korak 3: inclusive_scan -- i-ti element je zbir prvih i+1.
std::vector<double> kumulativno(const std::vector<double>& v) {
    std::vector<double> r(v.size());
    std::inclusive_scan(ex::par, v.begin(), v.end(), r.begin());
    return r;
}

int main() {
    std::vector<int> sirovo(105000);                     // 5000 x (40, 41, ..., 60)
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < sirovo.size(); ++i) sirovo[i] = static_cast<int>(40 + i % 21);   // 40..60
    auto v = kalibrisi(sirovo, 0.5, -10.0);                                                          // 10..20
    std::cout << "kalibrisano: prvi " << v.front() << ", poslednji " << v.back() << '\n';

    auto s = statistika(v, 19.0);
    std::cout << "prosek " << s.prosek << ", varijansa " << s.varijansa << ", iznad 19: " << s.iznadPraga << '\n';

    auto k = kumulativno(v);
    std::cout << "kumulativno: posle 3 merenja " << k[2] << ", ukupno " << k.back() << '\n';
    auto najveca = *std::max_element(ex::par, v.begin(), v.end());
    std::cout << "najveća vrednost: " << najveca << '\n';
}
