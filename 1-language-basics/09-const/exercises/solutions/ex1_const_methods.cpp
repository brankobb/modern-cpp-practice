// Rešenje zadatka ex1_const_methods.

#include <cstddef>
#include <iostream>
#include <vector>

class Readings {
public:
    void add(double t) {
        v_.push_back(t);
        cacheValid_ = false;
    }

    // Korak 1: sve što samo čita je const -- inače se ne može pozvati
    // preko const Readings& (kao u report()).
    std::size_t count() const { return v_.size(); }
    double last() const { return v_.back(); }

    // Korak 2: average() je logički const (spolja se ništa ne menja), a
    // keš je detalj implementacije -- zato mutable. Napomena (EMC Item 16):
    // ako se const metode zovu iz više niti, mutable keš treba zaštititi
    // (std::mutex ili std::atomic), jer "const" korisnik čita kao "bezbedno
    // za istovremeno čitanje".
    double average() const {
        if (!cacheValid_) {
            double s = 0;
            for (double x : v_) s += x;
            cache_ = s / static_cast<double>(v_.size());
            cacheValid_ = true;
            ++computations_;
        }
        return cache_;
    }
    int computations() const { return computations_; }

    // Korak 3: logika je samo u const verziji. Non-const pozove nju
    // (static_cast na const Readings&) i skine const sa rezultata -- to je
    // bezbedno, jer znamo da *this NIJE const objekat.
    const double& operator[](std::size_t i) const { return v_[i]; }
    double& operator[](std::size_t i) {
        cacheValid_ = false;   // pozivalac može da menja element
        return const_cast<double&>(static_cast<const Readings&>(*this)[i]);
    }

private:
    std::vector<double> v_;
    mutable double cache_ = 0;
    mutable bool cacheValid_ = false;
    mutable int computations_ = 0;
};

void report(const Readings& r) {
    std::cout << "count: " << r.count() << ", last: " << r.last()
              << ", average: " << r.average() << " (computations: " << r.computations() << ")\n";
}

int main() {
    Readings r;
    r.add(20.0);
    r.add(22.0);
    report(r);
    report(r);
    r.add(26.0);
    report(r);

    r[0] = 29.0;
    report(r);
    const Readings& cr = r;
    std::cout << "cr[1] = " << cr[1] << '\n';
}
