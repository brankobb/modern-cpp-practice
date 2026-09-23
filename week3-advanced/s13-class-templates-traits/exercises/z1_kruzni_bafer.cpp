// VRSTA: upotreba
//
// Zadatak 1 -- klasni šablon sa ne-tipskim parametrom, variadic metoda sa
// fold izrazom, static_assert i alias šablon (sekcije 2, 3, 6, 8)
//   ./build.sh week3-advanced/s13-class-templates-traits/exercises/z1_kruzni_bafer.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_kruzni_bafer.cpp
//
// Kružni bafer (ring buffer) je čest u embedded kodu: fiksan kapacitet,
// bez heap-a, a kad je pun, novi element pregazi najstariji.
// Korak 1: template <typename T, std::size_t N> class KruzniBafer --
//   std::array<T, N> podaci_, indeks najstarijeg (glava_) i broj elemenata.
//   void push(const T& v): kad je pun, pregazi najstariji (i pomeri glavu).
//   T pop(): vrati i ukloni najstariji; za prazan baci std::underflow_error.
//   std::size_t velicina() const, bool pun() const,
//   static constexpr std::size_t kapacitet().
//   static_assert(N > 0, "...") u klasi.
// Korak 2: template <typename... Args> void dodajSve(const Args&... args)
//   -- push za svaki argument, fold izrazom preko zareza (bez petlje i
//   rekurzije).
// Korak 3: alias šablon template <std::size_t N> using BajtBafer =
//   KruzniBafer<std::uint8_t, N>;

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

// TODO korak 1, 2, 3

int main() {
    std::cout << std::boolalpha;
    // Korak 1 i 2 -- otkomentariši:
    // KruzniBafer<int, 4> b;
    // b.dodajSve(1, 2, 3, 4, 5);
    // std::cout << "velicina " << b.velicina() << "/" << b.kapacitet() << ", pun: " << b.pun() << '\n';
    // std::cout << "pop:";
    // while (b.velicina() > 0) std::cout << ' ' << b.pop();
    // std::cout << '\n';
    // try {
    //     b.pop();
    // } catch (const std::underflow_error&) {
    //     std::cout << "prazan pop: underflow_error\n";
    // }

    // Korak 3 -- otkomentariši:
    // BajtBafer<3> bb;
    // bb.dodajSve(10, 20, 30, 40);
    // std::cout << "BajtBafer<3>:";
    // while (bb.velicina() > 0) std::cout << ' ' << int(bb.pop());
    // std::cout << '\n';
}

/* OČEKIVANI IZLAZ
velicina 4/4, pun: true
pop: 2 3 4 5
prazan pop: underflow_error
BajtBafer<3>: 20 30 40
*/
