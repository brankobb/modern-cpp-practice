// EXPECT-UB: ThreadSanitizer: data race
// SANITIZER: thread
// UB: std::async ne štiti deljene podatke. Dva zadatka sa
// launch::async rade u dve niti i menjaju isti brojac bez sinhronizacije
// -- isti data race kao lekcija 39, ub/u01.
// Ispravno: svaki zadatak vrati SVOJ rezultat kroz future, pa ih saberi
// posle get() (sekcija 1) -- ili mutex / std::atomic.
#include <future>
#include <iostream>
long counter = 0;
void job() {
    for (int i = 0; i < 100000; ++i) ++counter;
}
int main() {
    auto a = std::async(std::launch::async, job);
    auto b = std::async(std::launch::async, job);
    a.get();
    b.get();
    std::cout << counter << '\n';
}
