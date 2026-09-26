// KIND: why
// DEMO-OUT: NAIVE kopija: 1, pomeranja: 0
//
// Zadatak 3 -- zašto lokalna koju vraćaš ne treba da bude const (sekcija 2)
// Rešenje: exercises/solutions/ex3_const_lokalna.cpp
//
// izaberi() vraća jednu od dve lokalne, zavisno od uslova -- NRVO tada
// nije moguć (kompajler ne zna unapred koju da napravi u odredištu).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/24-copy-elision/exercises/ex3_const_lokalna.cpp -DNAIVE
//   Lokalne su const ("ne menjam ih, pa neka budu const"). Automatski
//   move na return-u tretira lokalnu kao rvalue: const Tekst&& -- move
//   konstruktor ga ne prima, pa je KOPIJA (isti razlog kao lekcija 22 ex3).
// Korak 2: u #else grani ukloni const sa lokalnih. Sada je 1 move.
// Korak 3: (za razmišljanje) kada je const lokalna u redu? (Kad je ne
//   vraćaš i ne pomeraš -- tada const samo pomaže.)

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

#ifdef NAIVE
Tekst izaberi(bool kratko) {
    const Tekst a("kratko");
    const Tekst b("dugačko");
    if (kratko) return a;
    return b;
}
#else
// TODO korak 2 (dok ne napišeš, ovo vraća prvalue)
Tekst izaberi(bool kratko) { return Tekst(kratko ? "kratko" : "dugačko"); }
#endif

int main() {
    Tekst r = izaberi(true);
    std::cout << r.s << " -- kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
}

/* EXPECTED OUTPUT
kratko -- kopija: 0, pomeranja: 1
*/
