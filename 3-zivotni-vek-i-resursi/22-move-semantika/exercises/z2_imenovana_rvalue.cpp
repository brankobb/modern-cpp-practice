// VRSTA: zašto
// DEMO-OUT: NAIVNO kopija: 1, pomeranja: 0
//
// Zadatak 2 -- zašto parametar T&& unutar funkcije treba std::move
// (sekcija 5)
// Rešenje: exercises/solutions/z2_imenovana_rvalue.cpp
//
// Poruka ima konstruktor koji prima Tekst&& -- "daj mi privremeni, ja ću
// ga preuzeti".
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-zivotni-vek-i-resursi/22-move-semantika/exercises/z2_imenovana_rvalue.cpp -DNAIVNO
//   Iako je argument privremeni i parametar je Tekst&&, član se KOPIRA.
//   Parametar t ima ime, pa je izraz "t" lvalue (kategorija vrednosti je
//   osobina IZRAZA, ne tipa). Kompajler ne sme sam da ga pomeri -- mogao
//   bi da se koristi i posle, u istom telu.
// Korak 2: u #else grani napiši konstruktor sa t_(std::move(t)).
// Korak 3: dodaj i konstruktor Poruka(const Tekst& t) za lvalue argumente
//   i proveri: lvalue -> 1 kopija, privremeni -> 1 pomeranje. (lekcija 24
//   pokazuje kraću varijantu: jedan konstruktor koji prima PO VREDNOSTI.)

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

#ifdef NAIVNO
struct Poruka {
    explicit Poruka(Tekst&& t) : t_(t) {}      // t je ovde lvalue!
    Tekst t_;
};

int main() {
    Poruka p(Tekst("zdravo"));
    std::cout << "kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // Poruka p(Tekst("zdravo"));
    // std::cout << "privremeni -> kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    // Tekst t("imenovan");
    // Poruka q(t);
    // std::cout << "lvalue -> kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    // std::cout << p.t_.s << ' ' << q.t_.s << ' ' << t.s << '\n';
}
#endif

/* OČEKIVANI IZLAZ
privremeni -> kopija: 0, pomeranja: 1
lvalue -> kopija: 1, pomeranja: 1
zdravo imenovan imenovan
*/
