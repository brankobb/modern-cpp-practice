// KIND: why
// DEMO-OUT: NAIVNO ukupna potrošnja: 3 kWh
//
// Zadatak 2 -- zašto accumulate "gubi" decimale (sekcija 2)
// Rešenje: exercises/solutions/ex2_accumulate_nula.cpp
//
// Brojilo daje potrošnju po satu u kWh; treba ukupna.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/36-algorithms/exercises/ex2_accumulate_nula.cpp -DNAIVNO
//   Zbir 0.5 + 1.5 + 2.5 ispadne 3 umesto 4.5.
//   accumulate je šablon: template <class It, class T> T accumulate(It, It, T init).
//   Tip zbira (i povratne vrednosti) je T -- tip POČETNE VREDNOSTI, ne
//   elemenata. 0 je int, pa se posle svakog sabiranja rezultat pretvori
//   nazad u int: 0 + 0.5 -> 0, 0 + 1.5 -> 1, 1 + 2.5 -> 3.
//   Bez upozorenja -- ni sa -Wall -Wextra, jer je konverzija u <numeric>.
// Korak 2: u #else grani napiši ukupno() tako da je zbir double.
//   (Ista zamka: accumulate(..., 0) nad long long ili size_t prelije int.)

#include <iostream>
#include <numeric>
#include <vector>

#ifdef NAIVNO
double ukupno(const std::vector<double>& v) { return std::accumulate(v.begin(), v.end(), 0); }
#else
// TODO korak 2 (dok ne napišeš, ova verzija vraća 0)
double ukupno(const std::vector<double>&) { return 0; }
#endif

int main() {
    std::vector<double> poSatu{0.5, 1.5, 2.5};
    std::cout << "ukupna potrošnja: " << ukupno(poSatu) << " kWh\n";
}

/* EXPECTED OUTPUT
ukupna potrošnja: 4.5 kWh
*/
