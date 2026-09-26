// Rešenje zadatka ex1_new_i_blok.

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

// Korak 1: new[] se oslobađa sa delete[] (ne delete, ne free -- ub/u01..u03).
// Vraćanje sirovog vlasničkog pokazivača prebacuje odgovornost na
// pozivaoca, i ništa ga ne tera da je ispuni.
int* kvadrati(std::size_t n) {
    int* p = new int[n];
    for (std::size_t i = 0; i < n; ++i) p[i] = static_cast<int>(i * i);
    return p;
}

// Korak 2: vlasništvo je u tipu -- pozivalac ne može da zaboravi delete[].
std::unique_ptr<int[]> kvadratiU(std::size_t n) {
    auto p = std::make_unique<int[]>(n);   // value-init: sve nule
    for (std::size_t i = 0; i < n; ++i) p[i] = static_cast<int>(i * i);
    return p;
}

// Korak 3: jedan blok umesto niza redova -- jedna alokacija, elementi jedan
// do drugog u memoriji (dobro za keš), i nema ručnog oslobađanja redova.
class Matrica {
public:
    Matrica(std::size_t redovi, std::size_t kolone)
        : redovi_(redovi), kolone_(kolone), podaci_(redovi * kolone) {}

    double& at(std::size_t r, std::size_t k) { return podaci_.at(r * kolone_ + k); }
    const double& at(std::size_t r, std::size_t k) const { return podaci_.at(r * kolone_ + k); }

    void ispisi() const {
        for (std::size_t r = 0; r < redovi_; ++r) {
            for (std::size_t k = 0; k < kolone_; ++k) std::cout << (k ? " " : "") << at(r, k);
            std::cout << '\n';
        }
    }

private:
    std::size_t redovi_;
    std::size_t kolone_;
    std::vector<double> podaci_;
};

int main() {
    int* a = kvadrati(5);
    for (std::size_t i = 0; i < 5; ++i) std::cout << (i ? " " : "") << a[i];
    std::cout << '\n';
    delete[] a;

    std::unique_ptr<int[]> b = kvadratiU(5);
    for (std::size_t i = 0; i < 5; ++i) std::cout << (i ? " " : "") << b[i];
    std::cout << '\n';

    Matrica m(2, 3);
    for (std::size_t r = 0; r < 2; ++r)
        for (std::size_t k = 0; k < 3; ++k) m.at(r, k) = static_cast<double>(10 * r + k);
    m.ispisi();
}
