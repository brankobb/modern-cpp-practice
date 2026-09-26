// KIND: why
// DEMO-OUT: NAIVE remaining from 100 after 30 and 20: 90
//
// Zadatak 2 -- zašto je bitno da li je fold levi ili desni (sekcija 2)
// Rešenje: exercises/solutions/ex2_fold_direction.cpp
//
// Budžet energije je 100; potrošači troše 30 i 20. Ostaje 50.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/29-cpp17-templates/exercises/ex2_fold_direction.cpp -DNAIVE
//   Ispadne 90. (a - ...) je DESNI fold: a1 - (a2 - a3) = 100 - (30 - 20).
//   Za + i * smer ne menja rezultat, pa se greška ne vidi dok se ne
//   upotrebi operator koji nije asocijativan (-, /, <<).
// Korak 2: u #else grani napiši remaining() levim fold-om, tako da bude
//   (a1 - a2) - a3.
// Korak 3: razmisli: šta vrati tvoja verzija za remaining(100), sa
//   jednim elementom? A za remaining() bez argumenata?

#include <iostream>

#ifdef NAIVE
template <typename... T>
int remaining(T... a) { return (a - ...); }
#else
// TODO korak 2 (dok ne napišeš, ova verzija vraća 0)
template <typename... T>
int remaining(T...) { return 0; }
#endif

int main() {
    std::cout << "remaining from 100 after 30 and 20: " << remaining(100, 30, 20) << '\n';
    std::cout << "remaining from 100 after 5, 5 and 10: " << remaining(100, 5, 5, 10) << '\n';
}

/* EXPECTED OUTPUT
remaining from 100 after 30 and 20: 50
remaining from 100 after 5, 5 and 10: 80
*/
