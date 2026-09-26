// Rešenje zadatka ex3_const_lokalna.

#include <iostream>
#include <utility>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

struct Tekst {
    const char* s;
    explicit Tekst(const char* x) : s(x) {}
    Tekst(const Tekst& o) : s(o.s) { ++brojac.kopija; }
    Tekst(Tekst&& o) noexcept : s(o.s) { ++brojac.pomeranja; }
};

// Ako su lokalne koje vraćaš const (nije dobro): automatski move na
// return-u ne može da ih pomeri, pa ih kopira.
// Treba ovako: lokalna koju vraćaš nije const. Kad NRVO nije moguć (dve
// različite lokalne), dobiješ move.
Tekst izaberi(bool kratko) {
    Tekst a("kratko");
    Tekst b("dugačko");
    if (kratko) return a;
    return b;
}

int main() {
    Tekst r = izaberi(true);
    std::cout << r.s << " -- kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
}
