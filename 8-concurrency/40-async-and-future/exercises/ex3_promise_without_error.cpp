// KIND: why
// SANITIZER: thread
// DEMO-OUT: NAIVE error: broken promise
//
// Zadatak 3 -- zašto promise mora da dobije i GREŠKU, ne samo vrednost
// (sekcije 5, 6)
// Rešenje: exercises/solutions/ex3_promise_without_error.cpp
//
// Radna nit računa koren merenja i šalje ga preko promise-a. Za
// negativno merenje ne može da izračuna.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 8-concurrency/40-async-and-future/exercises/ex3_promise_without_error.cpp -DNAIVE
//   Pozivalac dobije samo "broken promise": nit je izašla (return) bez
//   set_value, promise je uništen, a pravi razlog -- negativno merenje --
//   je izgubljen.
// Korak 2: u #else grani napiši compute() tako da za negativno merenje
//   pošalje izuzetak kroz promise:
//   p.set_exception(std::make_exception_ptr(std::domain_error(...)))
//   (ili throw u try, pa u catch set_exception(std::current_exception())).
//   Poruka: "negativno merenje: " + std::to_string(x).

#include <cmath>
#include <exception>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#ifdef NAIVE
void compute(std::promise<double> p, int x) {
    if (x < 0) return;
    p.set_value(std::sqrt(x));
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek šalje 0)
void compute(std::promise<double> p, int) { p.set_value(0); }
#endif

void attempt(int x) {
    std::promise<double> p;
    std::future<double> f = p.get_future();
    std::thread t(compute, std::move(p), x);   // promise je move-only: std::move
    try {
        double root = f.get();                // prvo get: ako baci, ništa se ne ispiše
        std::cout << "root(" << x << ") = " << root << '\n';
    } catch (const std::future_error& e) {
        std::cout << "error: " << (e.code() == std::future_errc::broken_promise ? "broken promise" : "future_error")
                  << '\n';
    } catch (const std::exception& e) {
        std::cout << "error: " << e.what() << '\n';
    }
    t.join();
}

int main() {
    attempt(16);
    attempt(-4);
}

/* EXPECTED OUTPUT
root(16) = 4
error: negative reading: -4
*/
