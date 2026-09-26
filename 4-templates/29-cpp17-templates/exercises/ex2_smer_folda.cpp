// KIND: why
// DEMO-OUT: NAIVE preostalo od 100 posle 30 i 20: 90
//
// Zadatak 2 -- zašto je bitno da li je fold levi ili desni (sekcija 2)
// Rešenje: exercises/solutions/ex2_smer_folda.cpp
//
// Budžet energije je 100; potrošači troše 30 i 20. Ostaje 50.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/29-cpp17-templates/exercises/ex2_smer_folda.cpp -DNAIVE
//   Ispadne 90. (a - ...) je DESNI fold: a1 - (a2 - a3) = 100 - (30 - 20).
//   Za + i * smer ne menja rezultat, pa se greška ne vidi dok se ne
//   upotrebi operator koji nije asocijativan (-, /, <<).
// Korak 2: u #else grani napiši preostalo() levim fold-om, tako da bude
//   (a1 - a2) - a3.
// Korak 3: razmisli: šta vrati tvoja verzija za preostalo(100), sa
//   jednim elementom? A za preostalo() bez argumenata?

#include <iostream>

#ifdef NAIVE
template <typename... T>
int preostalo(T... a) { return (a - ...); }
#else
// TODO korak 2 (dok ne napišeš, ova verzija vraća 0)
template <typename... T>
int preostalo(T...) { return 0; }
#endif

int main() {
    std::cout << "preostalo od 100 posle 30 i 20: " << preostalo(100, 30, 20) << '\n';
    std::cout << "preostalo od 100 posle 5, 5 i 10: " << preostalo(100, 5, 5, 10) << '\n';
}

/* EXPECTED OUTPUT
preostalo od 100 posle 30 i 20: 50
preostalo od 100 posle 5, 5 i 10: 80
*/
