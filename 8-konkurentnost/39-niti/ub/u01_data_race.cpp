// EXPECT-UB: ThreadSanitizer: data race
// SANITIZER: thread
// UB: dve niti menjaju isti int bez sinhronizacije -- data race
// ([intro.races]). ++brojac je "pročitaj, dodaj 1, upiši": kad se dve
// niti prepletu, jedno uvećanje se izgubi, pa rezultat obično bude MANJI
// od 200000 i razlikuje se od pokretanja do pokretanja. A pošto je UB,
// kompajler sme i gore (npr. da spoji petlju u jedno += 100000).
// ASan ovo NE vidi -- memorija je ispravna. Treba ThreadSanitizer:
//   ./build.sh 8-konkurentnost/39-niti/ub/u01_data_race.cpp --tsan
// Ispravno: std::mutex + std::lock_guard oko ++brojac (sekcije 5, 6),
// ili std::atomic<long> brojac.
#include <iostream>
#include <thread>
long brojac = 0;
void posao() {
    for (int i = 0; i < 100000; ++i) ++brojac;
}
int main() {
    std::thread t1(posao);
    std::thread t2(posao);
    t1.join();
    t2.join();
    std::cout << brojac << '\n';
}
