// KIND: why
// DEMO-OUT: NAIVE posle uklanjanja: 1 3 5 2 5 2 \(size 6\)
//
// Zadatak 3 -- zašto std::remove ne smanji vektor (sekcija 3)
// Rešenje: exercises/solutions/ex3_remove_ne_brise.cpp
//
// Iz liste kanala treba izbaciti isključeni kanal 2.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/36-algorithms/exercises/ex3_remove_ne_brise.cpp -DNAIVE
//   Veličina ostane 6, a kraj niza je "đubre" (stare vrednosti).
//   std::remove radi samo sa iteratorima -- ne zna za vektor, pa NE MOŽE
//   da mu promeni veličinu. Ono što ostaje prepiše ka početku i vrati
//   iterator na "novi kraj"; elementi od tog mesta do end() imaju
//   neodređene (ali važeće) vrednosti. Povratna vrednost je ovde ignorisana.
// Korak 2: u #else grani napiši ukloni() sa erase-remove idiomom: erase od
//   novog kraja do end(). (C++20: std::erase(v, x) radi oba koraka.)
// Korak 3: razmisli: zašto list ima svoju metodu l.remove(x), a vector ne?

#include <algorithm>
#include <iostream>
#include <vector>

#ifdef NAIVE
void ukloni(std::vector<int>& v, int x) { std::remove(v.begin(), v.end(), x); }
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne radi ništa)
void ukloni(std::vector<int>&, int) {}
#endif

int main() {
    std::vector<int> kanali{1, 2, 3, 2, 5, 2};
    ukloni(kanali, 2);
    std::cout << "posle uklanjanja:";
    for (int k : kanali) std::cout << ' ' << k;
    std::cout << " (size " << kanali.size() << ")\n";
}

/* EXPECTED OUTPUT
posle uklanjanja: 1 3 5 (size 3)
*/
