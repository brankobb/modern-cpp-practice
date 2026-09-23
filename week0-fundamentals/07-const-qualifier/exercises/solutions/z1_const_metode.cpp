// Rešenje zadatka z1_const_metode.

#include <cstddef>
#include <iostream>
#include <vector>

class Merenja {
public:
    void dodaj(double t) {
        v_.push_back(t);
        kesVazi_ = false;
    }

    // Korak 1: sve što samo čita je const -- inače se ne može pozvati
    // preko const Merenja& (kao u izvestaj()).
    std::size_t broj() const { return v_.size(); }
    double poslednja() const { return v_.back(); }

    // Korak 2: prosek() je logički const (spolja se ništa ne menja), a
    // keš je detalj implementacije -- zato mutable. Napomena (EMC Item 16):
    // ako se const metode zovu iz više niti, mutable keš treba zaštititi
    // (std::mutex ili std::atomic), jer "const" korisnik čita kao "bezbedno
    // za istovremeno čitanje".
    double prosek() const {
        if (!kesVazi_) {
            double s = 0;
            for (double x : v_) s += x;
            kes_ = s / static_cast<double>(v_.size());
            kesVazi_ = true;
            ++racunanja_;
        }
        return kes_;
    }
    int racunanja() const { return racunanja_; }

    // Korak 3: logika je samo u const verziji. Non-const pozove nju
    // (static_cast na const Merenja&) i skine const sa rezultata -- to je
    // bezbedno, jer znamo da *this NIJE const objekat.
    const double& operator[](std::size_t i) const { return v_[i]; }
    double& operator[](std::size_t i) {
        kesVazi_ = false;   // pozivalac može da menja element
        return const_cast<double&>(static_cast<const Merenja&>(*this)[i]);
    }

private:
    std::vector<double> v_;
    mutable double kes_ = 0;
    mutable bool kesVazi_ = false;
    mutable int racunanja_ = 0;
};

void izvestaj(const Merenja& m) {
    std::cout << "broj: " << m.broj() << ", poslednja: " << m.poslednja()
              << ", prosek: " << m.prosek() << " (računanja: " << m.racunanja() << ")\n";
}

int main() {
    Merenja m;
    m.dodaj(20.0);
    m.dodaj(22.0);
    izvestaj(m);
    izvestaj(m);
    m.dodaj(26.0);
    izvestaj(m);

    m[0] = 29.0;
    izvestaj(m);
    const Merenja& cm = m;
    std::cout << "cm[1] = " << cm[1] << '\n';
}
