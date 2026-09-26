// KIND: usage
//
// Zadatak 1 -- forwarding reference, std::forward i variadic template
// (sekcije 1, 3)
//   ./build.sh 4-templates/27-forwarding-and-lifetime/exercises/ex1_savrseno_prosledjivanje.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_savrseno_prosledjivanje.cpp
//
// Tekst broji kopije i pomeranja.
// Korak 1: template <typename T> void dodaj(std::vector<Tekst>& v, T&& x)
//   -- ubaci x u vektor tako da se lvalue KOPIRA, a rvalue POMERI:
//   v.push_back(std::forward<T>(x)). T&& u template-u je forwarding
//   referenca: T pamti da li je argument bio lvalue (T = Tekst&) ili
//   rvalue (T = Tekst).
// Korak 2: template <typename T, typename... Args>
//   std::unique_ptr<T> napravi(Args&&... args) -- sopstveni make_unique:
//   return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
//   Uredjaj(Tekst ime, int id) prima Tekst po vrednosti.
// Korak 3: template <typename F, typename... Args>
//   auto izmeri(F&& f, Args&&... args) -- ispiše "poziv" i vrati rezultat
//   std::forward<F>(f)(std::forward<Args>(args)...). Omotač ne sme da
//   doda nijednu kopiju.

#include <iostream>
#include <memory>
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
};

struct Uredjaj {
    Uredjaj(Tekst i, int broj) : ime(std::move(i)), id(broj) {}
    Tekst ime;
    int id;
};

std::size_t duzina(const Tekst& t) {
    std::size_t n = 0;
    while (t.s[n]) ++n;
    return n;
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::vector<Tekst> v;
    // v.reserve(2);
    // Tekst a("a");
    // dodaj(v, a);
    // brojac.ispisi("dodaj lvalue");
    // dodaj(v, Tekst("b"));
    // brojac.ispisi("dodaj rvalue");

    // Korak 2 -- otkomentariši:
    // auto u = napravi<Uredjaj>(Tekst("motor"), 7);
    // brojac.ispisi("napravi, rvalue");
    // auto w = napravi<Uredjaj>(a, 8);
    // brojac.ispisi("napravi, lvalue");
    // std::cout << u->ime.s << ' ' << u->id << ", " << w->ime.s << ' ' << w->id << '\n';

    // Korak 3 -- otkomentariši:
    // std::size_t n = izmeri(duzina, a);
    // std::cout << "dužina: " << n << '\n';
    // brojac.ispisi("izmeri");
}

/* EXPECTED OUTPUT
dodaj lvalue: kopija 1, move 0
dodaj rvalue: kopija 0, move 1
napravi, rvalue: kopija 0, move 2
napravi, lvalue: kopija 1, move 1
motor 7, a 8
poziv
dužina: 1
izmeri: kopija 0, move 0
*/
