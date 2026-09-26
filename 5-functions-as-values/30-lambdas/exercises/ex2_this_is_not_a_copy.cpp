// KIND: why
// DEMO-OUT: NAIVE 15 below threshold: false
//
// Zadatak 2 -- zašto [=] u metodi ne pravi snimak članova (sekcija 7)
// Rešenje: exercises/solutions/ex2_this_is_not_a_copy.cpp
//
// Thermostat pravi proveru "ispod praga" koja treba da važi sa pragom KOJI
// JE BIO kad je provera napravljena (npr. za pravilo zakazano unapred).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 5-functions-as-values/30-lambdas/exercises/ex2_this_is_not_a_copy.cpp -DNAIVE
//   Provera je napravljena sa pragom 20, prag je zatim promenjen na 10,
//   a provera koristi 10. [=] ne kopira članove: zarobi pokazivač this, pa
//   threshold_ u lambdi znači this->threshold_ -- uvek trenutna vrednost. (Da je
//   termostat uništen, bio bi i UB: ub/u01.) U C++20 je ovaj implicitni
//   capture zastareo -- probaj -std=c++20 i pročitaj upozorenje.
// Korak 2: u #else grani napiši makeCheck() koja stvarno pravi
//   snimak: init capture [threshold = threshold_]. (Možeš i ovako: [*this], C++17 --
//   kopija celog objekta.)

#include <iostream>

class Thermostat {
public:
    explicit Thermostat(int threshold) : threshold_(threshold) {}
    void setThreshold(int p) { threshold_ = p; }
    int threshold() const { return threshold_; }

#ifdef NAIVE
    auto makeCheck() const {
        return [=](int t) { return t < threshold_; };
    }
#else
    // TODO korak 2 (dok ne napišeš, ova verzija uvek vraća false)
    auto makeCheck() const {
        return [](int) { return false; };
    }
#endif

private:
    int threshold_;
};

int main() {
    std::cout << std::boolalpha;
    Thermostat ts(20);
    auto check = ts.makeCheck();
    std::cout << "check created with threshold " << ts.threshold() << '\n';
    ts.setThreshold(10);
    std::cout << "threshold now " << ts.threshold() << ", 15 below threshold: " << check(15) << '\n';
}

/* EXPECTED OUTPUT
check created with threshold 20
threshold now 10, 15 below threshold: true
*/
