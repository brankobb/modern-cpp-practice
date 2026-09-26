// Rešenje zadatka ex1_nadji_bag_gdb.
//
// Korak 1 (šta gdb pokaže): za v = {10, 20, 30, 40, 50} i k = 3,
// "print v.size() - k + 1" je 3, pa petlja krene od v[3]. watch s prvo
// pokaže inicijalizaciju (neko smeće -> 0: breakpoint na funkciji stane
// PRE reda "int s = 0;"), pa 0 -> 40 -> 90: u zbir uđu 40 i 50, a 30 je
// preskočen. Prosek je 90 / 3 = 30 umesto 40. Uzrok: "+ 1" -- prvi od
// poslednjih k elemenata je na indeksu size - k.

#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

double prosekPoslednjih(const std::vector<int>& v, std::size_t k) {
    // Korak 3: granice pre aritmetike sa size_t.
    if (k == 0) throw std::invalid_argument("k mora biti > 0");
    if (k > v.size()) k = v.size();
    int s = 0;
    for (std::size_t i = v.size() - k; i < v.size(); ++i) {   // Korak 2: bez + 1
        s += v[i];
    }
    return static_cast<double>(s) / static_cast<double>(k);
}

int main() {
    std::vector<int> v{10, 20, 30, 40, 50};
    std::cout << "prosek poslednja 3: " << prosekPoslednjih(v, 3) << '\n';
    std::cout << "prosek poslednjih 5: " << prosekPoslednjih(v, 5) << '\n';
    std::cout << "prosek poslednjih 7: " << prosekPoslednjih(v, 7) << '\n';
    try {
        prosekPoslednjih(v, 0);
    } catch (const std::invalid_argument& e) {
        std::cout << "k = 0: " << e.what() << '\n';
    }
}
