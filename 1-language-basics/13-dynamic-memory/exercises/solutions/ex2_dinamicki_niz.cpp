// Rešenje zadatka ex2_dinamicki_niz.

#include <cstddef>
#include <iostream>
#include <stdexcept>

class DinamickiNiz {
public:
    DinamickiNiz() = default;
    ~DinamickiNiz() { delete[] podaci_; }   // delete[] nullptr je dozvoljen (ne radi ništa)

    // Korak 1: vlasnik bloka se ne sme plitko kopirati. Pravu kopiju
    // (novi blok + prepisivanje) radi lekcija 20; ovde je zabranjena.
    DinamickiNiz(const DinamickiNiz&) = delete;
    DinamickiNiz& operator=(const DinamickiNiz&) = delete;

    // Korak 2: udvostručavanje daje amortizovano O(1) po dodavanju.
    // Novi blok se pravi PRE brisanja starog: ako new baci bad_alloc,
    // objekat ostaje nepromenjen (strong guarantee).
    void dodaj(int x) {
        if (vel_ == kap_) {
            std::size_t novi = kap_ ? 2 * kap_ : 1;
            int* blok = new int[novi];
            for (std::size_t i = 0; i < vel_; ++i) blok[i] = podaci_[i];
            delete[] podaci_;
            podaci_ = blok;
            std::cout << "rast: " << kap_ << " -> " << novi << '\n';
            kap_ = novi;
        }
        podaci_[vel_++] = x;
    }

    // Korak 3
    int at(std::size_t i) const {
        if (i >= vel_) throw std::out_of_range("DinamickiNiz::at");
        return podaci_[i];
    }
    std::size_t velicina() const { return vel_; }
    std::size_t kapacitet() const { return kap_; }

private:
    int* podaci_ = nullptr;
    std::size_t vel_ = 0;
    std::size_t kap_ = 0;
};

int main() {
    DinamickiNiz n;
    for (int i = 1; i <= 10; ++i) n.dodaj(i);
    int zbir = 0;
    for (std::size_t i = 0; i < n.velicina(); ++i) zbir += n.at(i);
    std::cout << "velicina: " << n.velicina() << ", kapacitet: " << n.kapacitet()
              << ", zbir: " << zbir << '\n';
    try {
        n.at(10);
    } catch (const std::out_of_range&) {
        std::cout << "at(10): out_of_range\n";
    }
}
