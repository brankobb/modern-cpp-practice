// KIND: usage
//
// Zadatak 1 -- obrada velikog niza merenja paralelnim algoritmima
// (sekcije 1, 2, 3)
//   ./build.sh 8-concurrency/41-parallel-algorithms/exercises/ex1_obrada_merenja.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_obrada_merenja.cpp
//
// Nijedan korak ne sme da ima deljenu promenljivu koju lambda menja --
// svaki rezultat skuplja algoritam.
//
// Korak 1: std::vector<double> kalibrisi(const std::vector<int>& sirovo, double k, double n)
//   -- k * x + n za svaki element; std::transform(std::execution::par, ...).
// Korak 2: struct Statistika { double prosek, varijansa; long iznadPraga; };
//   Statistika statistika(const std::vector<double>& v, double prag)
//   -- prosek: std::reduce(par); varijansa: prosek od (x - prosek)^2 preko
//   std::transform_reduce(par, ..., 0.0, std::plus<>{}, lambda);
//   iznadPraga: std::count_if(par).
// Korak 3: std::vector<double> kumulativno(const std::vector<double>& v)
//   -- std::inclusive_scan(par).
// (Ako je TBB instaliran, build.sh sam doda -ltbb; bez njega sve radi
// sekvencijalno, sa istim izlazom.)

#include <algorithm>
#include <execution>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace ex = std::execution;

// TODO korak 1, 2, 3

int main() {
    std::vector<int> sirovo(105000);                     // 5000 x (40, 41, ..., 60)
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < sirovo.size(); ++i) sirovo[i] = static_cast<int>(40 + i % 21);   // 40..60

    // Korak 1 -- otkomentariši:
    // auto v = kalibrisi(sirovo, 0.5, -10.0);                                                       // 10..20
    // std::cout << "kalibrisano: prvi " << v.front() << ", poslednji " << v.back() << '\n';

    // Korak 2 -- otkomentariši:
    // auto s = statistika(v, 19.0);
    // std::cout << "prosek " << s.prosek << ", varijansa " << s.varijansa << ", iznad 19: " << s.iznadPraga << '\n';

    // Korak 3 -- otkomentariši:
    // auto k = kumulativno(v);
    // std::cout << "kumulativno: posle 3 merenja " << k[2] << ", ukupno " << k.back() << '\n';
    // auto najveca = *std::max_element(ex::par, v.begin(), v.end());
    // std::cout << "najveća vrednost: " << najveca << '\n';
}

/* EXPECTED OUTPUT
kalibrisano: prvi 10.00, poslednji 20.00
prosek 15.00, varijansa 9.17, iznad 19: 10000
kumulativno: posle 3 merenja 31.50, ukupno 1575000.00
najveća vrednost: 20.00
*/
