// Rešenje zadatka z1_parametri_i_povratak.

#include <iostream>
#include <utility>
#include <vector>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
    void ispisi(const char* opis) {
        std::cout << opis << ": kopija " << kopija << ", move " << pomeranja << '\n';
        *this = Brojac{};
    }
} brojac;

struct Tekst {
    const char* s;
    explicit Tekst(const char* x) : s(x) {}
    Tekst(const Tekst& o) : s(o.s) { ++brojac.kopija; }
    Tekst(Tekst&& o) noexcept : s(o.s) { ++brojac.pomeranja; }
    Tekst& operator=(const Tekst& o) {
        s = o.s;
        ++brojac.kopija;
        return *this;
    }
    Tekst& operator=(Tekst&& o) noexcept {
        s = o.s;
        ++brojac.pomeranja;
        return *this;
    }
};

// Korak 1: return prvalue -- objekat se pravi direktno u odredištu
// (C++17 garantovano, i za tipove bez kopije i move-a).
Tekst napravi(const char* s) { return Tekst(s); }

// NRVO: lokalna promenljiva se pravi direktno u odredištu. Nije
// garantovano, ali kad ga nema, return lokalne je automatski move.
Tekst napraviImenovan(const char* s) {
    Tekst t(s);
    return t;
}

// Korak 2: jedan "sink" po vrednosti umesto para const& / && overload-a.
// Cena: jedan move više nego sa dva overload-a -- obično zanemarljivo.
class Uredjaj {
public:
    void postaviIme(Tekst ime) { ime_ = std::move(ime); }

private:
    Tekst ime_{""};
};

int main() {
    Tekst a = napravi("a");
    brojac.ispisi("prvalue");
    Tekst b = napraviImenovan("b");
    brojac.ispisi("NRVO");

    Uredjaj u;
    u.postaviIme(a);
    brojac.ispisi("sink, lvalue");
    u.postaviIme(std::move(b));
    brojac.ispisi("sink, rvalue");

    std::vector<Tekst> v;
    v.reserve(2);
    v.push_back(Tekst("c"));
    brojac.ispisi("push_back");
    v.emplace_back("d");
    brojac.ispisi("emplace_back");
}
