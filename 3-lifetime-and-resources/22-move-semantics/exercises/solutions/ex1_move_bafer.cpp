// Rešenje zadatka ex1_move_bafer.

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

class Bafer {
public:
    explicit Bafer(std::size_t n) : n_(n), p_(new int[n]()) {}
    ~Bafer() { delete[] p_; }
    Bafer(const Bafer& o) : n_(o.n_), p_(new int[o.n_]) {
        std::copy(o.p_, o.p_ + n_, p_);
        ++brojac.kopija;
    }
    Bafer& operator=(const Bafer& o) {
        Bafer tmp(o);
        std::swap(n_, tmp.n_);
        std::swap(p_, tmp.p_);
        return *this;
    }

    // Korak 1: bez alokacije -- samo preuzmi pokazivač. std::exchange
    // vrati staru vrednost i upiše novu, u jednom koraku.
    // noexcept: vector tada pri rastu pomera umesto da kopira (lekcija 23).
    Bafer(Bafer&& o) noexcept : n_(std::exchange(o.n_, 0)), p_(std::exchange(o.p_, nullptr)) {
        ++brojac.pomeranja;
    }

    // Korak 2: bez provere, a = std::move(a) bi oslobodio sopstveni blok,
    // pa ga "preuzeo" -- visi pokazivač.
    Bafer& operator=(Bafer&& o) noexcept {
        if (this != &o) {
            delete[] p_;
            n_ = std::exchange(o.n_, 0);
            p_ = std::exchange(o.p_, nullptr);
            ++brojac.pomeranja;
        }
        return *this;
    }

    std::size_t velicina() const { return n_; }

private:
    std::size_t n_;
    int* p_;
};

Bafer napravi(std::size_t n) {
    Bafer b(n);
    return b;
}

int main() {
    std::vector<Bafer> v;
    v.reserve(2);
    Bafer a(100);
    v.push_back(std::move(a));
    // Moved-from objekat je validan (sme da se uništi, dodeli, pita za
    // veličinu), samo je prazan -- tako smo ga napisali.
    std::cout << "a posle move: " << a.velicina() << ", u vektoru: " << v[0].velicina() << '\n';
    v.push_back(napravi(50));
    Bafer x(1), y(2);
    Bafer t = std::move(x);
    x = std::move(y);
    y = std::move(t);
    std::cout << "x: " << x.velicina() << ", y: " << y.velicina() << '\n';
    Bafer& isti = x;
    x = std::move(isti);
    std::cout << "x posle x = move(x): " << x.velicina() << '\n';
    std::cout << "kopija: " << brojac.kopija << '\n';
}
