// KIND: why
// DEMO-UB: NAIVE runtime error|SEGV
//
// Zadatak 2 -- zašto redosled u init listi ne odlučuje ništa
// (sekcija 2, "Redosled inicijalizacije", EC++ Item 4, C.47)
// Rešenje: exercises/solutions/ex2_member_order.cpp
//
// Sensor ima Calibration i Filter, a Filter se pravi IZ Calibration.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/14-class-basics/exercises/ex2_member_order.cpp -DNAIVE
//   Init lista kaže "prvo cal_, pa filt_", ali članovi se prave redom
//   DEKLARACIJE u klasi ([class.base.init]), a filt_ je deklarisan prvi.
//   Filter čita cal_.coef pre nego što je cal_ napravljen. Sensor je na
//   heap-u, a ASan novu heap memoriju puni bajtom 0xbe, pa UBSan prijavi
//   pristup adresi 0xbebebebebebebebe -- to je "pokazivač" vektora koji
//   još ne postoji.
//   Pročitaj i upozorenja: g++ -Wreorder i -Wuninitialized, clang
//   -Wreorder-ctor. Kompajler zna -- zato build bez upozorenja.
// Korak 2: u #else grani popravi: deklaracije članova u redosledu
//   zavisnosti (cal_ pa filt_), i init lista u istom redosledu.

#include <iostream>
#include <memory>
#include <vector>

struct Calibration {
    std::vector<double> coef;
    Calibration() : coef{2.0, 0.5} { std::cout << "Calibration\n"; }
};

struct Filter {
    double gain;
    explicit Filter(const Calibration& c) : gain(c.coef[0]) { std::cout << "Filter\n"; }
};

#ifdef NAIVE
struct Sensor {
    Filter filt_;          // deklarisan PRVI -> pravi se prvi
    Calibration cal_;
    Sensor() : cal_(), filt_(cal_) {}
};
#else
struct Sensor {
    // TODO korak 2
};
#endif

int main() {
    auto s = std::make_unique<Sensor>();
    (void)s;
    // Korak 2 -- otkomentariši:
    // std::cout << "gain: " << s->filt_.gain << '\n';
#ifdef NAIVE
    std::cout << "gain: " << s->filt_.gain << '\n';
#endif
}

/* EXPECTED OUTPUT
Calibration
Filter
gain: 2
*/
