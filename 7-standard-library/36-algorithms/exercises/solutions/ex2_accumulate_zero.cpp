// Rešenje zadatka ex2_accumulate_zero.

#include <iostream>
#include <numeric>
#include <vector>

// Ako napišeš accumulate(..., 0) (nije dobro): T je int, pa se svaki
// međuzbir odseče na ceo broj -- 3 umesto 4.5.
// Treba ovako: početna vrednost tipa double (0.0), pa je i zbir double.
double total(const std::vector<double>& v) { return std::accumulate(v.begin(), v.end(), 0.0); }

int main() {
    std::vector<double> perHour{0.5, 1.5, 2.5};
    std::cout << "total consumption: " << total(perHour) << " kWh\n";
}
