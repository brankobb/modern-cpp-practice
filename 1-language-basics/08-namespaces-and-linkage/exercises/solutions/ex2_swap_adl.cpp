// Rešenje zadatka ex2_swap_adl.
//
// Korak 3: ADL traži funkciju u namespace-ima TIPOVA argumenata. Za
// geo::Image to je geo, pa swap mora biti tamo (ovde kao "hidden friend":
// definisan u klasi, vidljiv samo ADL-om). U globalnom namespace-u bi ga
// našao samo poziv iz globalnog koda -- std::sort, std::reverse i ostali
// algoritmi koji interno zovu swap(a, b) ga ne bi videli.

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>

int copies = 0;
int geoSwap = 0;

namespace geo {

class Image {
public:
    explicit Image(std::size_t n) : n_(n), px_(new int[n]()) {}
    Image(const Image& o) : n_(o.n_), px_(new int[o.n_]) {
        std::copy(o.px_, o.px_ + n_, px_);
        ++copies;
    }
    Image& operator=(const Image& o) {
        if (this != &o) {
            int* fresh = new int[o.n_];
            std::copy(o.px_, o.px_ + o.n_, fresh);
            delete[] px_;
            px_ = fresh;
            n_ = o.n_;
            ++copies;
        }
        return *this;
    }
    ~Image() { delete[] px_; }

    std::size_t size() const { return n_; }
    friend void swap(Image& a, Image& b) noexcept {
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
    geo::Image a(1000), b(2000);
    // Ako pišeš "std::swap(a, b);" (nije dobro): kvalifikovan poziv ne
    // gleda u namespace geo, pa se pozove generički std::swap. Image nema
    // move, pa on napravi tri KOPIJE bafera.
    // Treba ovako: using-deklaracija daje std::swap kao rezervu, a
    // nekvalifikovan poziv uključi ADL, koji nađe geo::swap za geo::Image.
    using std::swap;
    swap(a, b);
    std::cout << "a: " << a.size() << ", b: " << b.size() << '\n';
    std::cout << "copies: " << copies << ", geo::swap: " << geoSwap << '\n';
}
