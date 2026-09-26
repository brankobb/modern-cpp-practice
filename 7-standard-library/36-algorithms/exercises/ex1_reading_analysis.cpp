// KIND: usage
//
// Zadatak 1 -- mali STL projekat: analiza loga merenja (sekcije 2, 3, 4;
// kurs 187)
//   ./build.sh 7-standard-library/36-algorithms/exercises/ex1_reading_analysis.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_reading_analysis.cpp
//
// Log je niz redova "senzor vrednost". Bez ručnih petlji za računanje --
// svaki korak je jedan ili dva algoritma.
//
// Korak 1: std::map<std::string, std::vector<double>> load(const std::vector<std::string>& log)
//   -- std::istringstream, >> senzor >> vrednost; red koji se ne pročita
//   (nema broja) preskoči.
// Korak 2: struct Stats { double min, max, mean, median; };
//   Stats computeStats(std::vector<double> v)   (po vrednosti -- zašto?)
//   -- min i max jednim pozivom: std::minmax_element;
//   -- mean: std::accumulate (pazi na početnu vrednost, zadatak ex2);
//   -- medijana: element na indeksu size()/2 posle std::nth_element.
// Korak 3: std::vector<double> largestN(const mapa&, std::size_t n)
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
        "temp 21.5", "humidity 40",   "temp 22.0", "temp 35.5", "humidity 42",
        "temp 21.0", "humidity 90",   "temp 22.5", "humidity 44",  "invalid",
    };
    (void)log;
    std::cout << std::fixed << std::setprecision(1);

    // Korak 1 -- otkomentariši:
    // auto m = load(log);
    // for (const auto& [name, v] : m) std::cout << name << ": " << v.size() << " readings\n";

    // Korak 2 -- otkomentariši:
    // for (const auto& [name, v] : m) {
    //     Stats s = computeStats(v);
    //     std::cout << name << ": min " << s.min << ", max " << s.max << ", mean " << s.mean
    //               << ", median " << s.median << '\n';
    // }

    // Korak 3 -- otkomentariši:
    // std::cout << "3 largest values:";
    // for (double x : largestN(m, 3)) std::cout << ' ' << x;
    // std::cout << '\n';

    // Korak 4 -- otkomentariši:
    // std::cout << "temp above 30:";
    // for (double x : above(m.at("temp"), 30)) std::cout << ' ' << x;
    // std::cout << "\nhumidity above 80: " << above(m.at("humidity"), 80).size() << '\n';
}

/* EXPECTED OUTPUT
humidity: 4 readings
temp: 5 readings
humidity: min 40.0, max 90.0, mean 54.0, median 44.0
temp: min 21.0, max 35.5, mean 24.5, median 22.0
3 largest values: 90.0 44.0 42.0
temp above 30: 35.5
humidity above 80: 1
*/
