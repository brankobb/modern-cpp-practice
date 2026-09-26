// VRSTA: upotreba
//
// Zadatak 1 -- funkcijski šabloni, dedukcija i ne-tipski parametri
// (sekcije 1, 2, 4, 6)
//   ./build.sh 4-sabloni/26-funkcijski-sabloni/exercises/z1_genericke_funkcije.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_genericke_funkcije.cpp
//
// Korak 1: template <typename T> T ogranici(T v, T lo, T hi) -- vrati v
//   ograničen na [lo, hi]. Koristi samo operator< (kao std::clamp), da bi
//   radilo za svaki tip koji ga ima. Zašto ogranici(15, 0, 10.0) ne
//   prolazi, a ogranici<double>(15, 0, 10.0) prolazi?
// Korak 2: template <typename T, std::size_t N> T zbir(const std::array<T, N>& a)
//   i template <typename T, std::size_t N> constexpr T najveci(const T (&niz)[N]).
//   N se dedukuje iz tipa argumenta. najveci mora da radi i u
//   static_assert-u (constexpr, C++17 dozvoljava petlje u constexpr).
// Korak 3: template <int Pojacanje> int skaliraj(int x) -- x * Pojacanje,
//   sa static_assert(Pojacanje > 0, "...") u telu. Probaj skaliraj<0>(5):
//   greška pri kompajliranju, sa tvojom porukom (pa vrati u komentar).

#include <array>
#include <cstddef>
#include <iostream>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << "ogranici(15, 0, 10) = " << ogranici(15, 0, 10) << '\n';
    // std::cout << "ogranici(2.5, 0.0, 1.0) = " << ogranici(2.5, 0.0, 1.0) << '\n';
    // std::cout << "ogranici<double>(15, 0, 10.0) = " << ogranici<double>(15, 0, 10.0) << '\n';

    // Korak 2 -- otkomentariši:
    // std::array<int, 4> a{1, 2, 3, 4};
    // std::cout << "zbir = " << zbir(a) << '\n';
    // constexpr int niz[] = {3, 9, 2};
    // static_assert(najveci(niz) == 9);
    // std::cout << "najveci = " << najveci(niz) << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << "skaliraj<4>(5) = " << skaliraj<4>(5) << '\n';
}

/* OČEKIVANI IZLAZ
ogranici(15, 0, 10) = 10
ogranici(2.5, 0.0, 1.0) = 1
ogranici<double>(15, 0, 10.0) = 10
zbir = 10
najveci = 9
skaliraj<4>(5) = 20
*/
