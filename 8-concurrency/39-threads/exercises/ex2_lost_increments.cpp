// KIND: why
// SANITIZER: thread
// DEMO-UB: NAIVE ThreadSanitizer: data race
//
// Zadatak 2 -- zašto ++ nad deljenom promenljivom treba mutex (sekcije 5, 6; ub/u01)
// Rešenje: exercises/solutions/ex2_lost_increments.cpp
//
// Četiri niti broje impulse sa istog senzora u jedan brojač.
//
// Korak 1: pokreni naivnu verziju, prvo bez TSan-a pa sa njim:
//     ./build.sh 8-concurrency/39-threads/exercises/ex2_lost_increments.cpp -DNAIVE
//     ./build.sh 8-concurrency/39-threads/exercises/ex2_lost_increments.cpp -DNAIVE --tsan
//   Bez TSan-a zbir je obično manji od 200000 i menja se od pokretanja do
//   pokretanja (ponekad i tačan -- zato se data race teško nađe testom).
//   ++vrednost je tri koraka: pročitaj, dodaj 1, upiši. Dve niti pročitaju
//   isto, obe upišu isto+1 -- jedno uvećanje nestane. TSan to prijavi kao
//   "data race" i kad zbir slučajno ispadne tačan.
// Korak 2: u #else grani dodaj std::mutex član i zaključaj ga
//   std::lock_guard-om u increment() i u read().
//   (Druga mogućnost: std::atomic<long> -- za jedan brojač i brže.)

#include <iostream>
#include <thread>
#include <vector>
#ifndef NAIVE
#include <mutex>
#endif

#ifdef NAIVE
class Counter {
public:
    void increment() { ++value_; }
    long read() const { return value_; }

private:
    long value_ = 0;
};
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne broji ništa)
class Counter {
public:
    void increment() {}
    long read() const { return 0; }
};
#endif

int main() {
    Counter b;
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
        threads.emplace_back([&b] {
            for (int j = 0; j < 50000; ++j) b.increment();
        });
    for (auto& t : threads) t.join();
    std::cout << "pulses: " << b.read() << '\n';
}

/* EXPECTED OUTPUT
pulses: 200000
*/
