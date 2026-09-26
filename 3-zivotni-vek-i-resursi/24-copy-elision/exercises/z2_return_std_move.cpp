// VRSTA: zašto
// DEMO-OUT: NAIVNO kopija: 0, pomeranja: 1
//
// Zadatak 2 -- zašto NE pisati return std::move(lokalna) (sekcije 1, 2)
// Rešenje: exercises/solutions/z2_return_std_move.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-zivotni-vek-i-resursi/24-copy-elision/exercises/z2_return_std_move.cpp -DNAIVNO
//   Ideja "pomeriću je da se ne kopira" daje GORI rezultat: 1 move umesto
//   0. std::move(t) nije ime lokalne promenljive nego izraz tipa Tekst&&,
//   pa NRVO više ne može da se primeni -- ostaje samo move. Pročitaj i
//   upozorenje: g++ i clang (-Wpessimizing-move, u -Wall) ga daju.
// Korak 2: u #else grani napiši napravi() sa običnim return t;
//   Ako NRVO nije moguć, kompajler sam uradi move (sekcija 2) -- std::move
//   na return-u lokalne nikad ne pomaže.

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

#ifdef NAIVNO
Tekst napravi() {
    Tekst t("rezultat");
    return std::move(t);
}
#else
// TODO korak 2 (dok ne napišeš, ovo vraća prvalue)
Tekst napravi() { return Tekst("rezultat"); }
#endif

int main() {
    Tekst r = napravi();
    std::cout << r.s << " -- kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
}

/* OČEKIVANI IZLAZ
rezultat -- kopija: 0, pomeranja: 0
*/
