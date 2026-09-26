// KIND: usage
//
// Zadatak 1 -- funkcijski šabloni, dedukcija i ne-tipski parametri
// (sekcije 1, 2, 4, 6)
//   ./build.sh 4-templates/26-function-templates/exercises/ex1_generic_functions.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_generic_functions.cpp
//
// Korak 1: template <typename T> T clampTo(T v, T lo, T hi) -- vrati v
//   ograničen na [lo, hi]. Koristi samo operator< (kao std::clamp), da bi
//   radilo za svaki tip koji ga ima. Zašto clampTo(15, 0, 10.0) ne
//   prolazi, a clampTo<double>(15, 0, 10.0) prolazi?
// Korak 2: template <typename T, std::size_t N> T sum(const std::array<T, N>& a)
//   i template <typename T, std::size_t N> constexpr T largest(const T (&arr)[N]).
//   N se dedukuje iz tipa argumenta. largest mora da radi i u
//   static_assert-u (constexpr, C++17 dozvoljava petlje u constexpr).
// Korak 3: template <int Gain> int scale(int x) -- x * Gain,
//   sa static_assert(Gain > 0, "...") u telu. Probaj scale<0>(5):
//   greška pri kompajliranju, sa tvojom porukom (pa vrati u komentar).

#include <array>
#include <cstddef>
#include <iostream>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << "clampTo(15, 0, 10) = " << clampTo(15, 0, 10) << '\n';
    // std::cout << "clampTo(2.5, 0.0, 1.0) = " << clampTo(2.5, 0.0, 1.0) << '\n';
    // std::cout << "clampTo<double>(15, 0, 10.0) = " << clampTo<double>(15, 0, 10.0) << '\n';

    // Korak 2 -- otkomentariši:
    // std::array<int, 4> a{1, 2, 3, 4};
    // std::cout << "sum = " << sum(a) << '\n';
    // constexpr int arr[] = {3, 9, 2};
    // static_assert(largest(arr) == 9);
    // std::cout << "largest = " << largest(arr) << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "scale<4>(5) = " << scale<4>(5) << '\n';
}

/* EXPECTED OUTPUT
clampTo(15, 0, 10) = 10
clampTo(2.5, 0.0, 1.0) = 1
clampTo<double>(15, 0, 10.0) = 10
sum = 10
largest = 9
scale<4>(5) = 20
*/
