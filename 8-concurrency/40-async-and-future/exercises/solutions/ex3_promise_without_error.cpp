// Rešenje zadatka ex3_promise_without_error.

#include <cmath>
#include <exception>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

// Ako nit za grešku samo izađe (nije dobro): promise se uništi bez
// vrednosti, pa pozivalac dobije "broken promise" umesto pravog razloga.
// Treba ovako: i greška ide kroz promise -- set_exception.
void compute(std::promise<double> p, int x) {
    if (x < 0) {
        p.set_exception(std::make_exception_ptr(std::domain_error("negative reading: " + std::to_string(x))));
        return;
    }
    p.set_value(std::sqrt(x));
}

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
