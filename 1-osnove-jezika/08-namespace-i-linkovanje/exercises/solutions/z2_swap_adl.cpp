// Rešenje zadatka z2_swap_adl.
//
// Korak 3: ADL traži funkciju u namespace-ima TIPOVA argumenata. Za
// geo::Slika to je geo, pa swap mora biti tamo (ovde kao "hidden friend":
// definisan u klasi, vidljiv samo ADL-om). U globalnom namespace-u bi ga
// našao samo poziv iz globalnog koda -- std::sort, std::reverse i ostali
// algoritmi koji interno zovu swap(a, b) ga ne bi videli.

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
    // Ako pišeš "std::swap(a, b);" (nije dobro): kvalifikovan poziv ne
    // gleda u namespace geo, pa se pozove generički std::swap. Slika nema
    // move, pa on napravi tri KOPIJE bafera.
    // Treba ovako: using-deklaracija daje std::swap kao rezervu, a
    // nekvalifikovan poziv uključi ADL, koji nađe geo::swap za geo::Slika.
    using std::swap;
    swap(a, b);
    std::cout << "a: " << a.velicina() << ", b: " << b.velicina() << '\n';
    std::cout << "kopiranja: " << kopiranja << ", geo::swap: " << geoSwap << '\n';
}
