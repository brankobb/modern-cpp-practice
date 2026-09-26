// Rešenje zadatka ex2_izgubljena_uvecanja.

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Ako je ++vrednost_ bez zaštite (nije dobro): niti se prepliću između
// čitanja i upisa, uvećanja se gube, a po standardu je to data race -- UB.
// Treba ovako: mutex čuva vrednost_; i čitanje ide pod njim.
class Brojac {
public:
    void uvecaj() {
        std::lock_guard<std::mutex> g(m_);
        ++vrednost_;
    }
    long procitaj() const {
        std::lock_guard<std::mutex> g(m_);
        return vrednost_;
    }

private:
    mutable std::mutex m_;
    long vrednost_ = 0;
};

int main() {
    Brojac b;
    std::vector<std::thread> niti;
    for (int i = 0; i < 4; ++i)
        niti.emplace_back([&b] {
            for (int j = 0; j < 50000; ++j) b.uvecaj();
        });
    for (auto& t : niti) t.join();
    std::cout << "impulsa: " << b.procitaj() << '\n';
}
