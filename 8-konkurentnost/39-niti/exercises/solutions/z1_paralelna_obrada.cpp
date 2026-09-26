// Rešenje zadatka z1_paralelna_obrada.

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
long paralelniZbir(const std::vector<int>& v, std::size_t n) {
    std::vector<long> delovi(n);
    std::vector<std::thread> niti;
    const std::size_t korak = v.size() / n;
    for (std::size_t i = 0; i < n; ++i) {
        auto od = v.begin() + static_cast<std::ptrdiff_t>(i * korak);
        auto doKraja = (i + 1 == n) ? v.end() : od + static_cast<std::ptrdiff_t>(korak);
        niti.emplace_back([od, doKraja, &delovi, i] { delovi[i] = std::accumulate(od, doKraja, 0L); });
    }
    for (auto& t : niti) t.join();
    return std::accumulate(delovi.begin(), delovi.end(), 0L);
}

// Korak 2: deljeni podaci + mutex u istoj klasi; svaki pristup pod
// lock_guard-om. mutable: i const metoda mora da zaključa.
class BezbedanDnevnik {
public:
    void zapisi(std::string s) {
        std::lock_guard<std::mutex> g(m_);
        zapisi_.push_back(std::move(s));
    }
    std::vector<std::string> sortirano() const {
        std::lock_guard<std::mutex> g(m_);
        auto kopija = zapisi_;
        std::sort(kopija.begin(), kopija.end());
        return kopija;
    }

private:
    mutable std::mutex m_;
    std::vector<std::string> zapisi_;
};

// Korak 3: join u destruktoru -- radi i kad izuzetak preskoči ostatak funkcije.
class CuvarNiti {
public:
    explicit CuvarNiti(std::thread& t) : t_(t) {}
    ~CuvarNiti() {
        if (t_.joinable()) t_.join();
    }
    CuvarNiti(const CuvarNiti&) = delete;
    CuvarNiti& operator=(const CuvarNiti&) = delete;

private:
    std::thread& t_;
};

void obradiSaGreskom(int& rezultat) {
    std::thread t([&rezultat] { rezultat = 7; });
    CuvarNiti cuvar(t);
    throw std::runtime_error("greška posle pokretanja niti");
}

int main() {
    std::vector<int> v(1001);
    std::iota(v.begin(), v.end(), 0);   // 0..1000
    std::cout << "zbir, 1 nit: " << paralelniZbir(v, 1) << ", 4 niti: " << paralelniZbir(v, 4)
              << ", 7 niti: " << paralelniZbir(v, 7) << '\n';

    BezbedanDnevnik d;
    std::vector<std::thread> niti;
    for (int i = 0; i < 4; ++i)
        niti.emplace_back([&d, i] {
            for (int j = 0; j < 3; ++j) d.zapisi(std::to_string(i) + "." + std::to_string(j));
        });
    for (auto& t : niti) t.join();
    auto sve = d.sortirano();
    std::cout << "dnevnik: " << sve.size() << " zapisa, prvi " << sve.front() << ", poslednji " << sve.back()
              << '\n';

    int rezultat = 0;
    try {
        obradiSaGreskom(rezultat);
    } catch (const std::exception& e) {
        std::cout << "uhvaćeno: " << e.what() << "; nit je završila, rezultat " << rezultat << '\n';
    }
}
