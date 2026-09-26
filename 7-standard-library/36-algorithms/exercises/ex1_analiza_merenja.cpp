// KIND: usage
//
// Zadatak 1 -- mali STL projekat: analiza loga merenja (sekcije 2, 3, 4;
// kurs 187)
//   ./build.sh 7-standard-library/36-algorithms/exercises/ex1_analiza_merenja.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_analiza_merenja.cpp
//
// Log je niz redova "senzor vrednost". Bez ručnih petlji za računanje --
// svaki korak je jedan ili dva algoritma.
//
// Korak 1: std::map<std::string, std::vector<double>> ucitaj(const std::vector<std::string>& log)
//   -- std::istringstream, >> senzor >> vrednost; red koji se ne pročita
//   (nema broja) preskoči.
// Korak 2: struct Statistika { double min, max, prosek, medijana; };
//   Statistika statistika(std::vector<double> v)   (po vrednosti -- zašto?)
//   -- min i max jednim pozivom: std::minmax_element;
//   -- prosek: std::accumulate (pazi na početnu vrednost, zadatak ex2);
//   -- medijana: element na indeksu size()/2 posle std::nth_element.
// Korak 3: std::vector<double> najvecih(const mapa&, std::size_t n)
//   -- sve vrednosti u jedan vektor (insert na kraj), pa std::partial_sort
//   prvih n opadajuće (std::greater<>{}), pa resize(n).
// Korak 4: std::vector<double> iznad(const std::vector<double>& v, double prag)
//   -- std::copy_if u prazan vektor preko std::back_inserter (ub/u01 i u02:
//   šta bude bez njega).

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// TODO korak 1, 2, 3, 4

int main() {
    std::vector<std::string> log{
        "temp 21.5", "vlaga 40",   "temp 22.0", "temp 35.5", "vlaga 42",
        "temp 21.0", "vlaga 90",   "temp 22.5", "vlaga 44",  "neispravno",
    };
    (void)log;
    std::cout << std::fixed << std::setprecision(1);

    // Korak 1 -- otkomentariši:
    // auto m = ucitaj(log);
    // for (const auto& [ime, v] : m) std::cout << ime << ": " << v.size() << " merenja\n";

    // Korak 2 -- otkomentariši:
    // for (const auto& [ime, v] : m) {
    //     Statistika s = statistika(v);
    //     std::cout << ime << ": min " << s.min << ", max " << s.max << ", prosek " << s.prosek
    //               << ", medijana " << s.medijana << '\n';
    // }

    // Korak 3 -- otkomentariši:
    // std::cout << "3 najveće vrednosti:";
    // for (double x : najvecih(m, 3)) std::cout << ' ' << x;
    // std::cout << '\n';

    // Korak 4 -- otkomentariši:
    // std::cout << "temp iznad 30:";
    // for (double x : iznad(m.at("temp"), 30)) std::cout << ' ' << x;
    // std::cout << "\nvlaga iznad 80: " << iznad(m.at("vlaga"), 80).size() << '\n';
}

/* EXPECTED OUTPUT
temp: 5 merenja
vlaga: 4 merenja
temp: min 21.0, max 35.5, prosek 24.5, medijana 22.0
vlaga: min 40.0, max 90.0, prosek 54.0, medijana 44.0
3 najveće vrednosti: 90.0 44.0 42.0
temp iznad 30: 35.5
vlaga iznad 80: 1
*/
