// KIND: why
// SANITIZER: thread
// DEMO-OUT: NAIVE greška: broken promise
//
// Zadatak 3 -- zašto promise mora da dobije i GREŠKU, ne samo vrednost
// (sekcije 5, 6)
// Rešenje: exercises/solutions/ex3_obecanje_bez_greske.cpp
//
// Radna nit računa koren merenja i šalje ga preko promise-a. Za
// negativno merenje ne može da izračuna.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 8-concurrency/40-async-and-future/exercises/ex3_obecanje_bez_greske.cpp -DNAIVE
//   Pozivalac dobije samo "broken promise": nit je izašla (return) bez
//   set_value, promise je uništen, a pravi razlog -- negativno merenje --
//   je izgubljen.
// Korak 2: u #else grani napiši racunaj() tako da za negativno merenje
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
void racunaj(std::promise<double> p, int x) {
    if (x < 0) return;
    p.set_value(std::sqrt(x));
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija uvek šalje 0)
void racunaj(std::promise<double> p, int) { p.set_value(0); }
#endif

void probaj(int x) {
    std::promise<double> p;
    std::future<double> f = p.get_future();
    std::thread t(racunaj, std::move(p), x);   // promise je move-only: std::move
    try {
        double koren = f.get();                // prvo get: ako baci, ništa se ne ispiše
        std::cout << "koren(" << x << ") = " << koren << '\n';
    } catch (const std::future_error& e) {
        std::cout << "greška: " << (e.code() == std::future_errc::broken_promise ? "broken promise" : "future_error")
                  << '\n';
    } catch (const std::exception& e) {
        std::cout << "greška: " << e.what() << '\n';
    }
    t.join();
}

int main() {
    probaj(16);
    probaj(-4);
}

/* EXPECTED OUTPUT
koren(16) = 4
greška: negativno merenje: -4
*/
