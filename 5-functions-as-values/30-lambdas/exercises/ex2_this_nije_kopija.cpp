// KIND: why
// DEMO-OUT: NAIVE 15 ispod praga: false
//
// Zadatak 2 -- zašto [=] u metodi ne pravi snimak članova (sekcija 7)
// Rešenje: exercises/solutions/ex2_this_nije_kopija.cpp
//
// Termostat pravi proveru "ispod praga" koja treba da važi sa pragom KOJI
// JE BIO kad je provera napravljena (npr. za pravilo zakazano unapred).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 5-functions-as-values/30-lambdas/exercises/ex2_this_nije_kopija.cpp -DNAIVE
//   Provera je napravljena sa pragom 20, prag je zatim promenjen na 10,
//   a provera koristi 10. [=] ne kopira članove: zarobi pokazivač this, pa
//   prag_ u lambdi znači this->prag_ -- uvek trenutna vrednost. (Da je
//   termostat uništen, bio bi i UB: ub/u01.) U C++20 je ovaj implicitni
//   capture zastareo -- probaj -std=c++20 i pročitaj upozorenje.
// Korak 2: u #else grani napiši napraviProveru() koja stvarno pravi
//   snimak: init capture [prag = prag_]. (Možeš i ovako: [*this], C++17 --
//   kopija celog objekta.)

#include <iostream>

class Termostat {
public:
    explicit Termostat(int prag) : prag_(prag) {}
    void postaviPrag(int p) { prag_ = p; }
    int prag() const { return prag_; }

#ifdef NAIVE
    auto napraviProveru() const {
        return [=](int t) { return t < prag_; };
    }
#else
    // TODO korak 2 (dok ne napišeš, ova verzija uvek vraća false)
    auto napraviProveru() const {
        return [](int) { return false; };
    }
#endif

private:
    int prag_;
};

int main() {
    std::cout << std::boolalpha;
    Termostat ts(20);
    auto provera = ts.napraviProveru();
    std::cout << "provera napravljena sa pragom " << ts.prag() << '\n';
    ts.postaviPrag(10);
    std::cout << "prag sada " << ts.prag() << ", 15 ispod praga: " << provera(15) << '\n';
}

/* EXPECTED OUTPUT
provera napravljena sa pragom 20
prag sada 10, 15 ispod praga: true
*/
