// KIND: why
// DEMO-OUT: NAIVE outside: general error
//
// Zadatak 2 -- zašto catch po const& i "throw;" (sekcije 1, 5)
// Rešenje: exercises/solutions/ex2_catch_by_value.cpp
//
// handle() uhvati grešku samo da je zabeleži, pa je prosledi dalje.
// Spolja se očekuje SensorError (sa id-jem senzora).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/18-exceptions/exercises/ex2_catch_by_value.cpp -DNAIVE
//   Spolja stiže "general error" -- SensorError je nestala.
//   a) catch (std::runtime_error e) -- PO VREDNOSTI: e je nova kopija
//      samo runtime_error dela (slicing, lekcija 16). g++ -Wall upozori
//      (-Wcatch-value), clang ćuti.
//   b) throw e; -- baca KOPIJU promenljive e, statičkog tipa
//      runtime_error. Originalni izuzetak je izgubljen.
// Korak 2: u #else grani napiši handle() ispravno: catch po const&, i
//   "throw;" -- ponovo baca ISTI objekat, sa njegovim pravim tipom.

#include <iostream>
#include <stdexcept>
#include <string>

class SensorError : public std::runtime_error {
public:
    SensorError(int sensorId, const std::string& message)
        : std::runtime_error("sensor " + std::to_string(sensorId) + ": " + message), id_(sensorId) {}
    int id() const noexcept { return id_; }

private:
    int id_;
};

#ifdef NAIVE
void handle() {
    try {
        throw SensorError(7, "timeout");
    } catch (std::runtime_error e) {
        std::cout << "log: " << e.what() << '\n';
        throw e;
    }
}
#else
// TODO korak 2 (dok ne napišeš, ova verzija ne radi ništa)
void handle() {}
#endif

int main() {
    try {
        handle();
    } catch (const SensorError& e) {
        std::cout << "outside: SensorError, id " << e.id() << '\n';
    } catch (const std::exception& e) {
        std::cout << "outside: general error (" << e.what() << ")\n";
    }
}

/* EXPECTED OUTPUT
log: sensor 7: timeout
outside: SensorError, id 7
*/
