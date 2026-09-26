// KIND: usage
//
// Zadatak 1 -- vraćanje po vrednosti, sink parametar, emplace_back
// (sekcije 1, 3, 4)
//   ./build.sh 3-lifetime-and-resources/24-copy-elision/exercises/ex1_parametri_i_povratak.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_parametri_i_povratak.cpp
//
// Tekst broji kopije i pomeranja (i konstruktorom i dodelom).
// Korak 1: Tekst napravi(const char* s) -- vrati PRVALUE: return Tekst(s);
//   i Tekst napraviImenovan(const char* s) -- Tekst t(s); return t;
//   Prvi je garantovana elizija (C++17), drugi NRVO (dozvoljen, ne
//   garantovan -- ali g++ i clang ga rade i na -O0).
// Korak 2: class Uredjaj sa std::string-olikim članom Tekst ime_ i
//   "sink" metodom void postaviIme(Tekst ime) { ime_ = std::move(ime); }
//   -- JEDNA funkcija, prima po vrednosti (F.18). Za lvalue argument:
//   1 kopija (u parametar) + 1 move (u član); za rvalue: 2 move-a.
// Korak 3: std::vector<Tekst> v; v.reserve(2); pa v.push_back(Tekst("a"))
//   i v.emplace_back("b"). emplace_back prosledi argumente konstruktoru i
//   napravi element NA MESTU -- nema privremenog objekta.

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

// TODO korak 1 i 2

int main() {
    // Korak 1 -- otkomentariši:
    // Tekst a = napravi("a");
    // brojac.ispisi("prvalue");
    // Tekst b = napraviImenovan("b");
    // brojac.ispisi("NRVO");

    // Korak 2 -- otkomentariši:
    // Uredjaj u;
    // u.postaviIme(a);
    // brojac.ispisi("sink, lvalue");
    // u.postaviIme(std::move(b));
    // brojac.ispisi("sink, rvalue");

    // Korak 3 -- otkomentariši:
    // std::vector<Tekst> v;
    // v.reserve(2);
    // v.push_back(Tekst("c"));
    // brojac.ispisi("push_back");
    // v.emplace_back("d");
    // brojac.ispisi("emplace_back");
}

/* EXPECTED OUTPUT
prvalue: kopija 0, move 0
NRVO: kopija 0, move 0
sink, lvalue: kopija 1, move 1
sink, rvalue: kopija 0, move 2
push_back: kopija 0, move 1
emplace_back: kopija 0, move 0
*/
