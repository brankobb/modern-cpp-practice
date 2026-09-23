// VRSTA: zašto
// SANITIZER: thread
// DEMO-UB: NAIVNO ThreadSanitizer: data race
//
// Zadatak 2 -- zašto ++ nad deljenom promenljivom treba mutex (sekcije 5, 6; ub/u01)
// Rešenje: exercises/solutions/z2_izgubljena_uvecanja.cpp
//
// Četiri niti broje impulse sa istog senzora u jedan brojač.
//
// Korak 1: pokreni naivnu verziju, prvo bez TSan-a pa sa njim:
//     ./build.sh week3-advanced/s19-threads/exercises/z2_izgubljena_uvecanja.cpp -DNAIVNO
//     ./build.sh week3-advanced/s19-threads/exercises/z2_izgubljena_uvecanja.cpp -DNAIVNO --tsan
//   Bez TSan-a zbir je obično manji od 200000 i menja se od pokretanja do
//   pokretanja (ponekad i tačan -- zato se data race teško nađe testom).
//   ++vrednost je tri koraka: pročitaj, dodaj 1, upiši. Dve niti pročitaju
//   isto, obe upišu isto+1 -- jedno uvećanje nestane. TSan to prijavi kao
//   "data race" i kad zbir slučajno ispadne tačan.
// Korak 2: u #else grani dodaj std::mutex član i zaključaj ga
//   std::lock_guard-om u uvecaj() i u procitaj().
//   (Druga mogućnost: std::atomic<long> -- za jedan brojač i brže.)

#include <iostream>
#include <thread>
#include <vector>
#ifndef NAIVNO
#include <mutex>
#endif

#ifdef NAIVNO
class Brojac {
public:
    void uvecaj() { ++vrednost_; }
    long procitaj() const { return vrednost_; }

private:
    long vrednost_ = 0;
};
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne broji ništa)
class Brojac {
public:
    void uvecaj() {}
    long procitaj() const { return 0; }
};
#endif

int main() {
    Brojac b;
    std::vector<std::thread> niti;
    for (int i = 0; i < 4; ++i)
        niti.emplace_back([&b] {
            for (int j = 0; j < 50000; ++j) b.uvecaj();
        });
    for (auto& t : niti) t.join();
    std::cout << "impulsa: " << b.procitaj() << '\n';
}

/* OČEKIVANI IZLAZ
impulsa: 200000
*/
