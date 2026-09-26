// Rešenje zadatka ex2_lost_increments.

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Ako je ++vrednost_ bez zaštite (nije dobro): niti se prepliću između
// čitanja i upisa, uvećanja se gube, a po standardu je to data race -- UB.
// Treba ovako: mutex čuva vrednost_; i čitanje ide pod njim.
class Counter {
public:
    void increment() {
        std::lock_guard<std::mutex> g(m_);
        ++value_;
    }
    long read() const {
        std::lock_guard<std::mutex> g(m_);
        return value_;
    }

private:
    mutable std::mutex m_;
    long value_ = 0;
};

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
