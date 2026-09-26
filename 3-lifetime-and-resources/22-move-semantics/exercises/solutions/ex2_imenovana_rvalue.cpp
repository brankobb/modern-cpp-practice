// Rešenje zadatka ex2_imenovana_rvalue.

#include <iostream>
#include <string>
#include <utility>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

struct Tekst {
    std::string s;
    explicit Tekst(std::string x) : s(std::move(x)) {}
    Tekst(const Tekst& o) : s(o.s) { ++brojac.kopija; }
    Tekst(Tekst&& o) noexcept : s(std::move(o.s)) { ++brojac.pomeranja; }
};

// Ako pišeš t_(t) za parametar Tekst&& t (nije dobro): t ima ime, pa je
// lvalue -- poziva se copy konstruktor.
// Treba ovako: std::move(t) -- "ovde mi t više ne treba, sme da se pomeri".
struct Poruka {
    explicit Poruka(Tekst&& t) : t_(std::move(t)) {}
    explicit Poruka(const Tekst& t) : t_(t) {}          // lvalue: kopija je ispravna
    Tekst t_;
};

int main() {
    Poruka p(Tekst("zdravo"));
    std::cout << "privremeni -> kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    Tekst t("imenovan");
    Poruka q(t);
    std::cout << "lvalue -> kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    std::cout << p.t_.s << ' ' << q.t_.s << ' ' << t.s << '\n';
}
