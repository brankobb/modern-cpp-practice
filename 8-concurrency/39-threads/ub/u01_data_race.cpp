// EXPECT-UB: ThreadSanitizer: data race
// SANITIZER: thread
// UB: dve niti menjaju isti int bez sinhronizacije -- data race
// ([intro.races]). ++brojac je "pročitaj, dodaj 1, upiši": kad se dve
// niti prepletu, jedno uvećanje se izgubi, pa rezultat obično bude MANJI
// od 200000 i razlikuje se od pokretanja do pokretanja. A pošto je UB,
// kompajler sme i gore (npr. da spoji petlju u jedno += 100000).
// ASan ovo NE vidi -- memorija je ispravna. Treba ThreadSanitizer:
//   ./build.sh 8-concurrency/39-threads/ub/u01_data_race.cpp --tsan
// Ispravno: std::mutex + std::lock_guard oko ++brojac (sekcije 5, 6),
// ili std::atomic<long> brojac.
#include <iostream>
#include <thread>
long counter = 0;
void work() {
    for (int i = 0; i < 100000; ++i) ++counter;
}
int main() {
    std::thread t1(work);
    std::thread t2(work);
    t1.join();
    t2.join();
    std::cout << counter << '\n';
}
