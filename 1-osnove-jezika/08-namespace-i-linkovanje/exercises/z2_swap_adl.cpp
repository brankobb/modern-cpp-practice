// VRSTA: zašto
// DEMO-OUT: NAIVNO kopiranja: 3, geo::swap: 0
//
// Zadatak 2 -- zašto "using std::swap; swap(a, b);" (sekcija 3, EC++ Item 25)
// Rešenje: exercises/solutions/z2_swap_adl.cpp
//
// geo::Slika drži veliki bafer i ima SAMO kopiju (C++03 stil, bez move-a),
// pa je kopiranje skupo. Zato uz tip postoji jeftin geo::swap koji samo
// zameni pokazivače.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-osnove-jezika/08-namespace-i-linkovanje/exercises/z2_swap_adl.cpp -DNAIVNO
//   std::swap(a, b) napravi 3 kopije, a geo::swap se ne pozove nijednom.
//   Zašto? Kvalifikovan poziv std::swap isključuje ADL.
// Korak 2: u #else grani zameni objekte idiomom
//       using std::swap;
//       swap(a, b);
//   Koliko je sada kopija? Isti idiom radi i za int (tada se koristi
//   std::swap), pa ga možeš pisati u generičkom kodu.
// Korak 3: odgovori u komentaru: zašto geo::swap mora da bude u namespace-u
//   geo, a ne u globalnom?

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>

int kopiranja = 0;
int geoSwap = 0;

namespace geo {

class Slika {
public:
    explicit Slika(std::size_t n) : n_(n), px_(new int[n]()) {}
    Slika(const Slika& o) : n_(o.n_), px_(new int[o.n_]) {
        std::copy(o.px_, o.px_ + n_, px_);
        ++kopiranja;
    }
    Slika& operator=(const Slika& o) {
        if (this != &o) {
            int* novi = new int[o.n_];
            std::copy(o.px_, o.px_ + o.n_, novi);
            delete[] px_;
            px_ = novi;
            n_ = o.n_;
            ++kopiranja;
        }
        return *this;
    }
    ~Slika() { delete[] px_; }

    std::size_t velicina() const { return n_; }
    friend void swap(Slika& a, Slika& b) noexcept {
        std::swap(a.n_, b.n_);
        std::swap(a.px_, b.px_);
        ++geoSwap;
    }

private:
    std::size_t n_;
    int* px_;
};

}  // namespace geo

int main() {
    geo::Slika a(1000), b(2000);
#ifdef NAIVNO
    std::swap(a, b);
#else
    // TODO korak 2
#endif
    std::cout << "a: " << a.velicina() << ", b: " << b.velicina() << '\n';
    std::cout << "kopiranja: " << kopiranja << ", geo::swap: " << geoSwap << '\n';
}

/* OČEKIVANI IZLAZ
a: 2000, b: 1000
kopiranja: 0, geo::swap: 1
*/
