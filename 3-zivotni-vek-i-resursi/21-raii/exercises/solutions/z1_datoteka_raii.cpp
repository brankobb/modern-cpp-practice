// Rešenje zadatka z1_datoteka_raii.

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

int zatvoreno = 0;
void zatvori(FILE* f) {
    if (f) {
        std::fclose(f);
        ++zatvoreno;
    }
}

// Korak 1: resurs se zauzme u konstruktoru, oslobodi u destruktoru. Ako
// konstruktor baci, resurs nije zauzet, pa nema šta da se oslobodi.
class Datoteka {
public:
    Datoteka() : f_(std::tmpfile()) {
        if (!f_) throw std::runtime_error("tmpfile nije uspeo");
    }
    ~Datoteka() { zatvori(f_); }
    Datoteka(const Datoteka&) = delete;              // dva vlasnika = dva fclose
    Datoteka& operator=(const Datoteka&) = delete;

    void pisi(const std::string& s) { std::fputs(s.c_str(), f_); }
    std::string procitajSve() {
        std::rewind(f_);
        std::string r;
        for (int c = std::fgetc(f_); c != EOF; c = std::fgetc(f_)) r += static_cast<char>(c);
        return r;
    }

private:
    FILE* f_;
};

// Korak 2: unique_ptr sa deleter-om je gotov RAII vlasnik za C resurse.
// Deleter kao prazan struct ne povećava sizeof (za razliku od pokazivača
// na funkciju kao deleter-a).
struct Zatvarac {
    void operator()(FILE* f) const { zatvori(f); }
};
using FilePtr = std::unique_ptr<FILE, Zatvarac>;

// Korak 3: najopštiji RAII -- "uradi ovo na izlasku iz bloka".
// C++17: tip se izvede iz konstruktora (CTAD), pa ScopeGuard([]{...}) radi
// bez <...>.
template <typename F>
class ScopeGuard {
public:
    explicit ScopeGuard(F f) : f_(std::move(f)) {}
    ~ScopeGuard() { f_(); }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;

private:
    F f_;
};

int main() {
    {
        Datoteka d;
        d.pisi("prvi red\n");
        d.pisi("drugi red");
        std::cout << "sadržaj: [" << d.procitajSve() << "]\n";
    }
    std::cout << "zatvoreno: " << zatvoreno << '\n';

    {
        FilePtr f(std::tmpfile());
        std::fputs("x", f.get());
    }
    std::cout << "zatvoreno: " << zatvoreno << '\n';

    try {
        auto g = ScopeGuard([] { std::cout << "guard: kraj bloka\n"; });
        throw std::runtime_error("greška u bloku");
    } catch (const std::exception& e) {
        std::cout << "uhvaćeno: " << e.what() << '\n';
    }
}
