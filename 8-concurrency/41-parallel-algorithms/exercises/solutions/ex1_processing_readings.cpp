// Rešenje zadatka ex1_processing_readings.

#include <algorithm>
#include <execution>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace ex = std::execution;

// Korak 1: svaki element u svoje mesto -- nema deljenja, pa nema ni race-a.
std::vector<double> calibrate(const std::vector<int>& raw, double k, double n) {
    std::vector<double> r(raw.size());
    std::transform(ex::par, raw.begin(), raw.end(), r.begin(), [k, n](int x) { return k * x + n; });
    return r;
}

// Korak 2: reduce i transform_reduce sami skupljaju; + je asocijativno.
// (Vrednosti su polovine celih brojeva, pa su zbirovi u double tačni i
// ne zavise od redosleda sabiranja -- inače bi par mogao da se razlikuje
// od seq u poslednjim ciframa.)
struct Stats {
    double mean, variance;
    long aboveThreshold;
};

Stats computeStats(const std::vector<double>& v, double threshold) {
    double n = static_cast<double>(v.size());
    double mean = std::reduce(ex::par, v.begin(), v.end(), 0.0) / n;
    double variance = std::transform_reduce(ex::par, v.begin(), v.end(), 0.0, std::plus<>{},
                                             [mean](double x) { return (x - mean) * (x - mean); }) / n;
    long above = std::count_if(ex::par, v.begin(), v.end(), [threshold](double x) { return x > threshold; });
    return {mean, variance, above};
}

// Korak 3: inclusive_scan -- i-ti element je zbir prvih i+1.
std::vector<double> cumulative(const std::vector<double>& v) {
    std::vector<double> r(v.size());
    std::inclusive_scan(ex::par, v.begin(), v.end(), r.begin());
    return r;
}

int main() {
    std::vector<int> raw(105000);                     // 5000 x (40, 41, ..., 60)
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < raw.size(); ++i) raw[i] = static_cast<int>(40 + i % 21);   // 40..60
    auto v = calibrate(raw, 0.5, -10.0);                                                          // 10..20
    std::cout << "calibrated: first " << v.front() << ", last " << v.back() << '\n';

    auto s = computeStats(v, 19.0);
    std::cout << "mean " << s.mean << ", variance " << s.variance << ", above 19: " << s.aboveThreshold << '\n';

    auto k = cumulative(v);
    std::cout << "cumulative: after 3 readings " << k[2] << ", total " << k.back() << '\n';
    auto largest = *std::max_element(ex::par, v.begin(), v.end());
    std::cout << "largest value: " << largest << '\n';
}
