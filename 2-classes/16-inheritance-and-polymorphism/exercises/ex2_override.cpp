// KIND: why
// DEMO-OUT: NAIVE Sensor::read -- default
// DEMO-ERR: OVERRIDE marked 'override', but does not override|marked 'override' hides virtual
//
// Zadatak 2 -- zašto override (sekcija 4, EMC Item 12)
// Rešenje: exercises/solutions/ex2_override.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/16-inheritance-and-polymorphism/exercises/ex2_override.cpp -DNAIVE
//   ThermoSensor "nadjačava" read(), ali je zaboravio const. To je nova
//   funkcija sa drugim potpisom -- NE nadjačava. Poziv preko Sensor& ide u
//   Sensor::read. Program se kompajlira; g++ 13 i clang daju samo
//   UPOZORENJE (-Woverloaded-virtual, u -Wall): "was hidden" / "hides
//   overloaded virtual function". Upozorenje se lako previdi među
//   ostalima, a program radi pogrešno.
// Korak 2: isto sa override:
//     ./build.sh .../ex2_override.cpp -DOVERRIDE
//   Sada je GREŠKA: g++ "marked 'override', but does not override",
//   clang "non-virtual member function marked 'override' hides virtual
//   member function".
// Korak 3: u #else grani napiši ThermoSensor ispravno, sa override. Ima
//   još jedan čest način da se potpis razlikuje, a da to ne primetiš: tip
//   parametra (int umesto long) -- probaj i njega sa override.

#include <iostream>

struct Sensor {
    virtual ~Sensor() = default;
    virtual double read() const {
        std::cout << "Sensor::read -- default\n";
        return 0.0;
    }
};

#if defined(NAIVE)
struct ThermoSensor : Sensor {
    double read() {                       // fali const: nova funkcija
        std::cout << "ThermoSensor::read\n";
        return 21.5;
    }
};
#elif defined(OVERRIDE)
struct ThermoSensor : Sensor {
    double read() override { return 21.5; }   // fali const: greška
};
#else
// TODO korak 3
#endif

void report(const Sensor& s) {
    double v = s.read();   // prvo očitaj (read() i sam ispisuje), pa ispiši
    std::cout << "value: " << v << '\n';
}

int main() {
#if defined(NAIVE) || defined(OVERRIDE)
    ThermoSensor t;
    report(t);
#else
    // Korak 3 -- otkomentariši:
    // ThermoSensor t;
    // report(t);
#endif
}

/* EXPECTED OUTPUT
ThermoSensor::read
value: 21.5
*/
