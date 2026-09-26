// KIND: why
// DEMO-OUT: NAIVE after removal: 1 3 5 2 5 2 \(size 6\)
//
// Zadatak 3 -- zašto std::remove ne smanji vektor (sekcija 3)
// Rešenje: exercises/solutions/ex3_remove_does_not_erase.cpp
//
// Iz liste kanala treba izbaciti isključeni kanal 2.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/36-algorithms/exercises/ex3_remove_does_not_erase.cpp -DNAIVE
//   Veličina ostane 6, a kraj niza je "đubre" (stare vrednosti).
//   std::remove radi samo sa iteratorima -- ne zna za vektor, pa NE MOŽE
//   da mu promeni veličinu. Ono što ostaje prepiše ka početku i vrati
//   iterator na "novi kraj"; elementi od tog mesta do end() imaju
//   neodređene (ali važeće) vrednosti. Povratna vrednost je ovde ignorisana.
// Korak 2: u #else grani napiši removeAll() sa erase-remove idiomom: erase od
//   novog kraja do end(). (C++20: std::erase(v, x) radi oba koraka.)
// Korak 3: razmisli: zašto list ima svoju metodu l.remove(x), a vector ne?

#include <algorithm>
#include <iostream>
#include <vector>

#ifdef NAIVE
void removeAll(std::vector<int>& v, int x) { std::remove(v.begin(), v.end(), x); }
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne radi ništa)
void removeAll(std::vector<int>&, int) {}
#endif

int main() {
    std::vector<int> channels{1, 2, 3, 2, 5, 2};
    removeAll(channels, 2);
    std::cout << "after removal:";
    for (int k : channels) std::cout << ' ' << k;
    std::cout << " (size " << channels.size() << ")\n";
}

/* EXPECTED OUTPUT
after removal: 1 3 5 (size 3)
*/
