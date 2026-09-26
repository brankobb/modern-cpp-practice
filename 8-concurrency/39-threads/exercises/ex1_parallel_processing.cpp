// KIND: usage
// SANITIZER: thread
//
// Zadatak 1 -- podela posla na niti, bezbedan deljeni dnevnik, join u
// destruktoru (sekcije 2, 4, 6; runtime/r01)
//   ./build.sh 8-concurrency/39-threads/exercises/ex1_parallel_processing.cpp
//   ./build.sh 8-concurrency/39-threads/exercises/ex1_parallel_processing.cpp --tsan
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla. Pokreni
// i sa --tsan: ThreadSanitizer ne sme ništa da prijavi.
// Rešenje: exercises/solutions/ex1_parallel_processing.cpp
//
// Korak 1: long parallelSum(const std::vector<int>& v, std::size_t n)
//   -- n niti, svaka sabira svoj deo (std::accumulate) u SVOJ element
//   std::vector<long> delovi(n); posle join-a saberi delove. Poslednja
//   nit uzima i ostatak (1001 element na 4 niti nije ravno).
// Korak 2: class SafeLog sa void write(std::string) i
//   std::vector<std::string> sortirano() const. Podaci i std::mutex su
//   članovi; svaki pristup pod std::lock_guard-om. (Zašto mutex mora da
//   bude mutable?)
// Korak 3: class ThreadGuard -- drži std::thread&, u destruktoru
//   if (joinable()) join(). U processWithError(int& result) pokreni nit
//   koja upiše 7 u rezultat, napravi čuvara, pa baci
//   std::runtime_error("greška posle pokretanja niti").
//   Bez čuvara: runtime/r01 (terminate). (C++20 std::jthread radi ovo sam.)

#include <algorithm>
#include <iostream>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    std::vector<int> v(1001);
    std::iota(v.begin(), v.end(), 0);   // 0..1000

    // Korak 1 -- otkomentariši:
    // std::cout << "sum, 1 thread: " << parallelSum(v, 1) << ", 4 threads: " << parallelSum(v, 4)
    //           << ", 7 threads: " << parallelSum(v, 7) << '\n';

    // Korak 2 -- otkomentariši:
    // SafeLog d;
    // std::vector<std::thread> threads;
    // for (int i = 0; i < 4; ++i)
    //     threads.emplace_back([&d, i] {
    //         for (int j = 0; j < 3; ++j) d.write(std::to_string(i) + "." + std::to_string(j));
    //     });
    // for (auto& t : threads) t.join();
    // auto all = d.sorted();
    // std::cout << "log: " << all.size() << " entries, first " << all.front() << ", last " << all.back()
    //           << '\n';

    // Korak 3 -- otkomentariši:
    // int result = 0;
    // try {
    //     processWithError(result);
    // } catch (const std::exception& e) {
    //     std::cout << "caught: " << e.what() << "; the thread finished, result " << result << '\n';
    // }
}

/* EXPECTED OUTPUT
sum, 1 thread: 500500, 4 threads: 500500, 7 threads: 500500
log: 12 entries, first 0.0, last 3.2
caught: error after starting the thread; the thread finished, result 7
*/
