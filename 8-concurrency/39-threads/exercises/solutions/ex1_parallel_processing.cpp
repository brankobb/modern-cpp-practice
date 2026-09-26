// Rešenje zadatka ex1_parallel_processing.

#include <algorithm>
#include <iostream>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// Korak 1: svaka nit sabira svoj deo u SVOJ element -- nema deljenja, nema
// mutex-a. Poslednja nit uzme i ostatak kad se v.size() ne deli sa n.
long parallelSum(const std::vector<int>& v, std::size_t n) {
    std::vector<long> parts(n);
    std::vector<std::thread> threads;
    const std::size_t step = v.size() / n;
    for (std::size_t i = 0; i < n; ++i) {
        auto from = v.begin() + static_cast<std::ptrdiff_t>(i * step);
        auto to = (i + 1 == n) ? v.end() : from + static_cast<std::ptrdiff_t>(step);
        threads.emplace_back([from, to, &parts, i] { parts[i] = std::accumulate(from, to, 0L); });
    }
    for (auto& t : threads) t.join();
    return std::accumulate(parts.begin(), parts.end(), 0L);
}

// Korak 2: deljeni podaci + mutex u istoj klasi; svaki pristup pod
// lock_guard-om. mutable: i const metoda mora da zaključa.
class SafeLog {
public:
    void write(std::string s) {
        std::lock_guard<std::mutex> g(m_);
        entries_.push_back(std::move(s));
    }
    std::vector<std::string> sorted() const {
        std::lock_guard<std::mutex> g(m_);
        auto copy = entries_;
        std::sort(copy.begin(), copy.end());
        return copy;
    }

private:
    mutable std::mutex m_;
    std::vector<std::string> entries_;
};

// Korak 3: join u destruktoru -- radi i kad izuzetak preskoči ostatak funkcije.
class ThreadGuard {
public:
    explicit ThreadGuard(std::thread& t) : t_(t) {}
    ~ThreadGuard() {
        if (t_.joinable()) t_.join();
    }
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;

private:
    std::thread& t_;
};

void processWithError(int& result) {
    std::thread t([&result] { result = 7; });
    ThreadGuard guard(t);
    throw std::runtime_error("error after starting the thread");
}

int main() {
    std::vector<int> v(1001);
    std::iota(v.begin(), v.end(), 0);   // 0..1000
    std::cout << "sum, 1 thread: " << parallelSum(v, 1) << ", 4 threads: " << parallelSum(v, 4)
              << ", 7 threads: " << parallelSum(v, 7) << '\n';

    SafeLog d;
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
        threads.emplace_back([&d, i] {
            for (int j = 0; j < 3; ++j) d.write(std::to_string(i) + "." + std::to_string(j));
        });
    for (auto& t : threads) t.join();
    auto all = d.sorted();
    std::cout << "log: " << all.size() << " entries, first " << all.front() << ", last " << all.back()
              << '\n';

    int result = 0;
    try {
        processWithError(result);
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << "; the thread finished, result " << result << '\n';
    }
}
