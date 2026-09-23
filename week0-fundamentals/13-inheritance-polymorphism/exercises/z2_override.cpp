// VRSTA: zašto
// DEMO-OUT: NAIVNO Senzor::ocitaj -- podrazumevano
// DEMO-ERR: OVERRIDE marked 'override', but does not override|marked 'override' hides virtual
//
// Zadatak 2 -- zašto override (sekcija 4, EMC Item 12)
// Rešenje: exercises/solutions/z2_override.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/13-inheritance-polymorphism/exercises/z2_override.cpp -DNAIVNO
//   TermoSenzor "nadjačava" ocitaj(), ali je zaboravio const. To je nova
//   funkcija sa drugim potpisom -- NE nadjačava. Poziv preko Senzor& ide u
//   Senzor::ocitaj. Program se kompajlira; g++ 13 i clang daju samo
//   UPOZORENJE (-Woverloaded-virtual, u -Wall): "was hidden" / "hides
//   overloaded virtual function". Upozorenje se lako previdi među
//   ostalima, a program radi pogrešno.
// Korak 2: isto sa override:
//     ./build.sh .../z2_override.cpp -DOVERRIDE
//   Sada je GREŠKA: g++ "marked 'override', but does not override",
//   clang "non-virtual member function marked 'override' hides virtual
//   member function".
// Korak 3: u #else grani napiši TermoSenzor ispravno, sa override. Ima
//   još jedan čest način da se potpis razlikuje, a da to ne primetiš: tip
//   parametra (int umesto long) -- probaj i njega sa override.

#include <iostream>

struct Senzor {
    virtual ~Senzor() = default;
    virtual double ocitaj() const {
        std::cout << "Senzor::ocitaj -- podrazumevano\n";
        return 0.0;
    }
};

#if defined(NAIVNO)
struct TermoSenzor : Senzor {
    double ocitaj() {                     // fali const: nova funkcija
        std::cout << "TermoSenzor::ocitaj\n";
        return 21.5;
    }
};
#elif defined(OVERRIDE)
struct TermoSenzor : Senzor {
    double ocitaj() override { return 21.5; }   // fali const: greška
};
#else
// TODO korak 3
#endif

void izvestaj(const Senzor& s) {
    double v = s.ocitaj();   // prvo očitaj (ocitaj() i sam ispisuje), pa ispiši
    std::cout << "vrednost: " << v << '\n';
}

int main() {
#if defined(NAIVNO) || defined(OVERRIDE)
    TermoSenzor t;
    izvestaj(t);
#else
    // Korak 3 -- otkomentariši:
    // TermoSenzor t;
    // izvestaj(t);
#endif
}

/* OČEKIVANI IZLAZ
TermoSenzor::ocitaj
vrednost: 21.5
*/
