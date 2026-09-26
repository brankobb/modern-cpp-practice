// Rešenje zadatka ex3_for_each_without_state.

#include <algorithm>
#include <execution>
#include <iostream>
#include <vector>

// Ako se broji funktorom kroz for_each(par) (nije dobro): paralelni
// for_each vraća void -- svaka nit ima svoju kopiju funktora, pa nema
// jednog stanja za vraćanje.
// Treba ovako: count_if sam skupi rezultate svih niti.
long alarms(const std::vector<double>& v, double threshold) {
    return std::count_if(std::execution::par, v.begin(), v.end(), [threshold](double x) { return x > threshold; });
}

int main() {
    std::vector<double> v(10000);
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = static_cast<double>(i % 100);   // 0..99
    std::cout << "above 90: " << alarms(v, 90.0) << '\n';
}
