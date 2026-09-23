// VRSTA: upotreba
//
// Zadatak 1 -- move konstruktor i move dodela (sekcije 4, 6)
//   ./build.sh week1-cpp03-to-move/s04-move-semantics/exercises/z1_move_bafer.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_move_bafer.cpp
//
// Bafer drži int niz na heap-u i već ima kopiju (rule of 3). Globalni
// brojači broje kopije i pomeranja.
// Korak 1: Bafer(Bafer&& o) noexcept -- "ukradi" pokazivač i veličinu od
//   o, a o ostavi PRAZAN (nullptr, 0) -- tako njegov destruktor ne
//   oslobodi ukradeni blok. Uvećaj brojac.pomeranja.
// Korak 2: Bafer& operator=(Bafer&& o) noexcept -- oslobodi svoj blok,
//   preuzmi o-ov, o ostavi praznim. Pazi na a = std::move(a)
//   (proveri this != &o).
// Korak 3: u testu: std::move u vector, vraćanje iz funkcije, zamena
//   dva bafera preko tri move-a (kao std::swap). Na kraju proveri da
//   nijedna kopija nije napravljena.

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

    // TODO korak 1 i 2

    std::size_t velicina() const { return n_; }

private:
    std::size_t n_;
    int* p_;
};

Bafer napravi(std::size_t n) {
    Bafer b(n);
    return b;           // NRVO ili automatski move -- nikad kopija (s07)
}

int main() {
    // Korak 3 -- otkomentariši (posle koraka 1 i 2):
    // std::vector<Bafer> v;
    // v.reserve(2);
    // Bafer a(100);
    // v.push_back(std::move(a));
    // std::cout << "a posle move: " << a.velicina() << ", u vektoru: " << v[0].velicina() << '\n';
    // v.push_back(napravi(50));
    // Bafer x(1), y(2);
    // Bafer t = std::move(x);
    // x = std::move(y);
    // y = std::move(t);
    // std::cout << "x: " << x.velicina() << ", y: " << y.velicina() << '\n';
    // Bafer& isti = x;
    // x = std::move(isti);                  // mora da preživi
    // std::cout << "x posle x = move(x): " << x.velicina() << '\n';
    // std::cout << "kopija: " << brojac.kopija << '\n';
}

/* OČEKIVANI IZLAZ
a posle move: 0, u vektoru: 100
x: 2, y: 1
x posle x = move(x): 2
kopija: 0
*/
