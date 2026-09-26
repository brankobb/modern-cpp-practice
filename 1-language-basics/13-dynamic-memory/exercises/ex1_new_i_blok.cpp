// KIND: usage
//
// Zadatak 1 -- new[]/delete[], unique_ptr<T[]> i 2D u jednom bloku
// (sekcije 5, 6, 7)
//   ./build.sh 1-language-basics/13-dynamic-memory/exercises/ex1_new_i_blok.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_new_i_blok.cpp
//
// Korak 1: int* kvadrati(std::size_t n) -- alocira niz od n int-ova sa
//   new[], popuni ga kvadratima 0, 1, 4, ... i vrati. Pozivalac ga
//   oslobađa -- kojim operatorom?
// Korak 2: isto, ali std::unique_ptr<int[]> kvadratiU(std::size_t n) sa
//   std::make_unique<int[]>(n) (elementi su nule). Nema ručnog delete-a.
// Korak 3: class Matrica (redovi x kolone double-ova) u JEDNOM bloku:
//   std::vector<double> podaci_(redovi * kolone). Metoda
//   double& at(std::size_t r, std::size_t k) računa indeks r * kolone + k
//   (i const verzija). Napiši i void ispisi() const.

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // int* a = kvadrati(5);
    // for (std::size_t i = 0; i < 5; ++i) std::cout << (i ? " " : "") << a[i];
    // std::cout << '\n';
    // delete[] a;

    // Korak 2 -- otkomentariši:
    // std::unique_ptr<int[]> b = kvadratiU(5);
    // for (std::size_t i = 0; i < 5; ++i) std::cout << (i ? " " : "") << b[i];
    // std::cout << '\n';

    // Korak 3 -- otkomentariši:
    // Matrica m(2, 3);
    // for (std::size_t r = 0; r < 2; ++r)
    //     for (std::size_t k = 0; k < 3; ++k) m.at(r, k) = static_cast<double>(10 * r + k);
    // m.ispisi();
}

/* EXPECTED OUTPUT
0 1 4 9 16
0 1 4 9 16
0 1 2
10 11 12
*/
