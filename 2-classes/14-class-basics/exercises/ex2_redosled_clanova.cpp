// KIND: why
// DEMO-UB: NAIVNO runtime error|SEGV
//
// Zadatak 2 -- zašto redosled u init listi ne odlučuje ništa
// (sekcija 2, "Redosled inicijalizacije", EC++ Item 4, C.47)
// Rešenje: exercises/solutions/ex2_redosled_clanova.cpp
//
// Senzor ima Kalibraciju i Filter, a Filter se pravi IZ Kalibracije.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/14-class-basics/exercises/ex2_redosled_clanova.cpp -DNAIVNO
//   Init lista kaže "prvo kal_, pa filt_", ali članovi se prave redom
//   DEKLARACIJE u klasi ([class.base.init]), a filt_ je deklarisan prvi.
//   Filter čita kal_.koef pre nego što je kal_ napravljen. Senzor je na
//   heap-u, a ASan novu heap memoriju puni bajtom 0xbe, pa UBSan prijavi
//   pristup adresi 0xbebebebebebebebe -- to je "pokazivač" vektora koji
//   još ne postoji.
//   Pročitaj i upozorenja: g++ -Wreorder i -Wuninitialized, clang
//   -Wreorder-ctor. Kompajler zna -- zato build bez upozorenja.
// Korak 2: u #else grani popravi: deklaracije članova u redosledu
//   zavisnosti (kal_ pa filt_), i init lista u istom redosledu.

#include <iostream>
#include <memory>
#include <vector>

struct Kalibracija {
    std::vector<double> koef;
    Kalibracija() : koef{2.0, 0.5} { std::cout << "Kalibracija\n"; }
};

struct Filter {
    double pojacanje;
    explicit Filter(const Kalibracija& k) : pojacanje(k.koef[0]) { std::cout << "Filter\n"; }
};

#ifdef NAIVNO
struct Senzor {
    Filter filt_;          // deklarisan PRVI -> pravi se prvi
    Kalibracija kal_;
    Senzor() : kal_(), filt_(kal_) {}
};
#else
struct Senzor {
    // TODO korak 2
};
#endif

int main() {
    auto s = std::make_unique<Senzor>();
    (void)s;
    // Korak 2 -- otkomentariši:
    // std::cout << "pojačanje: " << s->filt_.pojacanje << '\n';
#ifdef NAIVNO
    std::cout << "pojačanje: " << s->filt_.pojacanje << '\n';
#endif
}

/* EXPECTED OUTPUT
Kalibracija
Filter
pojačanje: 2
*/
