// KIND: why
// DEMO-OUT: NAIVE remaining from 100 after 10, 3, 2 and 1: 94
//
// Zadatak 2 -- zašto reduce nije "brži accumulate" (sekcije 2, 4)
// Rešenje: exercises/solutions/ex2_reduce_is_not_accumulate.cpp
//
// Budžet 100, troškovi 10, 3, 2 i 1 -- ostaje 84.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 8-concurrency/41-parallel-algorithms/exercises/ex2_reduce_is_not_accumulate.cpp -DNAIVE
//   Ispadne 94 -- a ovde je std::reduce čak BEZ politike (sekvencijalno).
//   reduce sme da grupiše i premešta operande (zato može paralelno);
//   libstdc++ to radi i sekvencijalno, po 4 elementa:
//   100 - ((10 - 3) - (2 - 1)) = 94. Za + i * je to svejedno, za - nije.
//   Standard traži da operacija bude asocijativna i komutativna; ako nije,
//   rezultat nije određen (drugačija biblioteka ili politika -> drugi broj;
//   sa TBB-om i par ovde ispadne 84, bez TBB-a 94).
// Korak 2: u #else grani napiši remaining() tako da bude tačno i da sme
//   paralelno: oduzimanje je "budžet minus ZBIR troškova", a zbir (+) je
//   asocijativan i komutativan.

#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <vector>

#ifdef NAIVE
int remaining(int budget, const std::vector<int>& t) {
    return std::reduce(t.begin(), t.end(), budget, [](int a, int b) { return a - b; });
}
#else
// TODO korak 2 (dok ne napišeš, vraća 0)
int remaining(int, const std::vector<int>&) { return 0; }
#endif

int main() {
    std::vector<int> costs{10, 3, 2, 1};
    std::cout << "remaining from 100 after 10, 3, 2 and 1: " << remaining(100, costs) << '\n';
    std::vector<int> many(1000, 1);
    std::cout << "remaining from 5000 after 1000 x 1: " << remaining(5000, many) << '\n';
}

/* EXPECTED OUTPUT
remaining from 100 after 10, 3, 2 and 1: 84
remaining from 5000 after 1000 x 1: 4000
*/
