// KIND: why
// DEMO-OUT: NAIVE generic sensor, value 0
//
// Zadatak 3 -- zašto se polimorfni objekti ne čuvaju po vrednosti (sekcija 8)
// Rešenje: exercises/solutions/ex3_slicing.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/16-inheritance-and-polymorphism/exercises/ex3_slicing.cpp -DNAIVE
//   Ubačena su dva ThermoSensor-a, a oba se ispišu kao "generic sensor".
//   std::vector<Sensor> čuva objekte tipa TAČNO Sensor: push_back kopira
//   samo Sensor deo ThermoSensor-a (slicing). Deo sa temperaturom i vptr
//   izvedene klase se ne kopiraju -- kopija je pravi Sensor.
// Korak 2: u #else grani čuvaj senzore kao
//   std::vector<std::unique_ptr<Sensor>> i ubacuj ih sa
//   std::make_unique<ThermoSensor>(...).
// Korak 3: zaštiti Sensor od slicing-a: copy konstruktor i copy dodela u
//   Sensor-u = delete (C.67). Tada se naivni kod više ne kompajlira --
//   probaj (lekcija 16, errors/e01).

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifdef NAIVE
struct Sensor {
    virtual ~Sensor() = default;
    virtual std::string describe() const { return "generic sensor, value 0"; }
};

struct ThermoSensor : Sensor {
    explicit ThermoSensor(double t) : temp(t) {}
    std::string describe() const override { return "thermo, " + std::to_string(temp); }
    double temp;
};

int main() {
    std::vector<Sensor> sensors;
    sensors.push_back(ThermoSensor(21.5));
    sensors.push_back(ThermoSensor(30.0));
    for (const Sensor& s : sensors) std::cout << s.describe() << '\n';
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 -- otkomentariši:
    // std::vector<std::unique_ptr<Sensor>> sensors;
    // sensors.push_back(std::make_unique<ThermoSensor>(21.5));
    // sensors.push_back(std::make_unique<ThermoSensor>(30.0));
    // for (const auto& s : sensors) std::cout << s->describe() << '\n';
}
#endif

/* EXPECTED OUTPUT
thermo, 21.5
thermo, 30.0
*/
