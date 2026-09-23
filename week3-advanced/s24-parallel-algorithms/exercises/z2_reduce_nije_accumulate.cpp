// VRSTA: zašto
// DEMO-OUT: NAIVNO preostalo od 100 posle 10, 3, 2 i 1: 94
//
// Zadatak 2 -- zašto reduce nije "brži accumulate" (sekcije 2, 4)
// Rešenje: exercises/solutions/z2_reduce_nije_accumulate.cpp
//
// Budžet 100, troškovi 10, 3, 2 i 1 -- ostaje 84.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week3-advanced/s24-parallel-algorithms/exercises/z2_reduce_nije_accumulate.cpp -DNAIVNO
//   Ispadne 94 -- a ovde je std::reduce čak BEZ politike (sekvencijalno).
//   reduce sme da grupiše i premešta operande (zato može paralelno);
//   libstdc++ to radi i sekvencijalno, po 4 elementa:
//   100 - ((10 - 3) - (2 - 1)) = 94. Za + i * je to svejedno, za - nije.
//   Standard traži da operacija bude asocijativna i komutativna; ako nije,
//   rezultat nije određen (drugačija biblioteka ili politika -> drugi broj;
//   sa TBB-om i par ovde ispadne 84, bez TBB-a 94).
// Korak 2: u #else grani napiši preostalo() tako da bude tačno i da sme
//   paralelno: oduzimanje je "budžet minus ZBIR troškova", a zbir (+) je
//   asocijativan i komutativan.

#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

#ifdef NAIVNO
int preostalo(int budzet, const std::vector<int>& t) {
    return std::reduce(t.begin(), t.end(), budzet, [](int a, int b) { return a - b; });
}
#else
// TODO korak 2 (dok ne napišeš, vraća 0)
int preostalo(int, const std::vector<int>&) { return 0; }
#endif

int main() {
    std::vector<int> troskovi{10, 3, 2, 1};
    std::cout << "preostalo od 100 posle 10, 3, 2 i 1: " << preostalo(100, troskovi) << '\n';
    std::vector<int> mnogo(1000, 1);
    std::cout << "preostalo od 5000 posle 1000 x 1: " << preostalo(5000, mnogo) << '\n';
}

/* OČEKIVANI IZLAZ
preostalo od 100 posle 10, 3, 2 i 1: 84
preostalo od 5000 posle 1000 x 1: 4000
*/
