// Rešenje zadatka z1_savrseno_prosledjivanje.

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

// Korak 1: jedna funkcija za oba slučaja. Bez std::forward bi x (ima ime)
// uvek bio lvalue, pa bi i rvalue bio kopiran; sa std::move bi i lvalue
// bio pomeren (z2).
template <typename T>
void dodaj(std::vector<Tekst>& v, T&& x) {
    v.push_back(std::forward<T>(x));
}

// Korak 2: svaki argument prosleđen sa svojom kategorijom. Brojke za
// rvalue: 1 move u parametar Uredjaj-a + 1 move u član = 2; za lvalue:
// kopija u parametar + move u član.
template <typename T, typename... Args>
std::unique_ptr<T> napravi(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Korak 3: omotač koji "ne postoji" za argumente -- sve prosledi dalje
// po referenci, sa istom kategorijom. decltype(auto) bi čuvao i referencu
// u povratnom tipu; ovde je dovoljno auto.
template <typename F, typename... Args>
auto izmeri(F&& f, Args&&... args) {
    std::cout << "poziv\n";
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

int main() {
    std::vector<Tekst> v;
    v.reserve(2);
    Tekst a("a");
    dodaj(v, a);
    brojac.ispisi("dodaj lvalue");
    dodaj(v, Tekst("b"));
    brojac.ispisi("dodaj rvalue");

    auto u = napravi<Uredjaj>(Tekst("motor"), 7);
    brojac.ispisi("napravi, rvalue");
    auto w = napravi<Uredjaj>(a, 8);
    brojac.ispisi("napravi, lvalue");
    std::cout << u->ime.s << ' ' << u->id << ", " << w->ime.s << ' ' << w->id << '\n';

    std::size_t n = izmeri(duzina, a);
    std::cout << "dužina: " << n << '\n';
    brojac.ispisi("izmeri");
}
