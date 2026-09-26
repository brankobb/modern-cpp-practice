// Rešenje zadatka ex1_generic_functions.

#include <array>
#include <cstddef>
#include <iostream>

// Korak 1: jedan T za sva tri parametra. clampTo(15, 0, 10.0) daje
// T = int iz prva dva i T = double iz trećeg -- konflikt (errors/e01).
// Sa <double> T je zadat, pa se 15 i 0 obično konvertuju.
template <typename T>
T clampTo(T v, T lo, T hi) {
    if (v < lo) return lo;
    if (hi < v) return hi;
    return v;
}

// Korak 2: N je deo tipa (std::array<T, N>, T[N]), pa se dedukuje.
template <typename T, std::size_t N>
T sum(const std::array<T, N>& a) {
    T s{};
    for (const T& x : a) s += x;
    return s;
}

template <typename T, std::size_t N>
constexpr T largest(const T (&arr)[N]) {
    static_assert(N > 0, "an empty array has no largest element");
    T m = arr[0];
    for (std::size_t i = 1; i < N; ++i)
        if (m < arr[i]) m = arr[i];
    return m;
}

// Korak 3: static_assert na ne-tipskom parametru -- pogrešna vrednost je
// greška pri kompajliranju, sa porukom koju si napisao.
template <int Gain>
int scale(int x) {
    static_assert(Gain > 0, "gain must be positive");
    return x * Gain;
}

int main() {
    std::cout << "clampTo(15, 0, 10) = " << clampTo(15, 0, 10) << '\n';
    std::cout << "clampTo(2.5, 0.0, 1.0) = " << clampTo(2.5, 0.0, 1.0) << '\n';
    std::cout << "clampTo<double>(15, 0, 10.0) = " << clampTo<double>(15, 0, 10.0) << '\n';

    std::array<int, 4> a{1, 2, 3, 4};
    std::cout << "sum = " << sum(a) << '\n';
    constexpr int arr[] = {3, 9, 2};
    static_assert(largest(arr) == 9);
    std::cout << "largest = " << largest(arr) << '\n';

    std::cout << "scale<4>(5) = " << scale<4>(5) << '\n';
}
