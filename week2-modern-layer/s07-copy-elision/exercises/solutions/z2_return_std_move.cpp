// Rešenje zadatka z2_return_std_move.

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

// Ako pišeš "return std::move(t);" (nije dobro): NRVO se isključi, pa
// umesto nula dobiješ jedan move.
// Treba ovako: "return t;" -- NRVO, a gde nije moguć, automatski move.
Tekst napravi() {
    Tekst t("rezultat");
    return t;
}

int main() {
    Tekst r = napravi();
    std::cout << r.s << " -- kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
}
