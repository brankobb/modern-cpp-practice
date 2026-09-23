// Rešenje zadatka z3_for_each_bez_stanja.

#include <algorithm>
#include <execution>
#include <iostream>
#include <vector>

// Ako se broji funktorom kroz for_each(par) (nije dobro): paralelni
// for_each vraća void -- svaka nit ima svoju kopiju funktora, pa nema
// jednog stanja za vraćanje.
// Treba ovako: count_if sam skupi rezultate svih niti.
long alarmi(const std::vector<double>& v, double prag) {
    return std::count_if(std::execution::par, v.begin(), v.end(), [prag](double x) { return x > prag; });
}

int main() {
    std::vector<double> v(10000);
    for (std::size_t i = 0; i < v.size(); ++i) v[i] = static_cast<double>(i % 100);   // 0..99
    std::cout << "iznad 90: " << alarmi(v, 90.0) << '\n';
}
