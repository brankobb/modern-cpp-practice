// Rešenje zadatka z1_genericke_funkcije.

#include <array>
#include <cstddef>
#include <iostream>

// Korak 1: jedan T za sva tri parametra. ogranici(15, 0, 10.0) daje
// T = int iz prva dva i T = double iz trećeg -- konflikt (errors/e01).
// Sa <double> T je zadat, pa se 15 i 0 obično konvertuju.
template <typename T>
T ogranici(T v, T lo, T hi) {
    if (v < lo) return lo;
    if (hi < v) return hi;
    return v;
}

// Korak 2: N je deo tipa (std::array<T, N>, T[N]), pa se dedukuje.
template <typename T, std::size_t N>
T zbir(const std::array<T, N>& a) {
    T s{};
    for (const T& x : a) s += x;
    return s;
}

template <typename T, std::size_t N>
constexpr T najveci(const T (&niz)[N]) {
    static_assert(N > 0, "prazan niz nema najveći element");
    T m = niz[0];
    for (std::size_t i = 1; i < N; ++i)
        if (m < niz[i]) m = niz[i];
    return m;
}

// Korak 3: static_assert na ne-tipskom parametru -- pogrešna vrednost je
// greška pri kompajliranju, sa porukom koju si napisao.
template <int Pojacanje>
int skaliraj(int x) {
    static_assert(Pojacanje > 0, "pojačanje mora biti pozitivno");
    return x * Pojacanje;
}

int main() {
    std::cout << "ogranici(15, 0, 10) = " << ogranici(15, 0, 10) << '\n';
    std::cout << "ogranici(2.5, 0.0, 1.0) = " << ogranici(2.5, 0.0, 1.0) << '\n';
    std::cout << "ogranici<double>(15, 0, 10.0) = " << ogranici<double>(15, 0, 10.0) << '\n';

    std::array<int, 4> a{1, 2, 3, 4};
    std::cout << "zbir = " << zbir(a) << '\n';
    constexpr int niz[] = {3, 9, 2};
    static_assert(najveci(niz) == 9);
    std::cout << "najveci = " << najveci(niz) << '\n';

    std::cout << "skaliraj<4>(5) = " << skaliraj<4>(5) << '\n';
}
