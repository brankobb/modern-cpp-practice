// Rešenje zadatka ex3_slicing.

#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// Korak 3: bazna klasa ne može da se kopira, pa slicing ne može da se
// desi slučajno (ni push_back u vector<Sensor>, ni parametar po vrednosti).
struct Sensor {
    Sensor() = default;
    Sensor(const Sensor&) = delete;
    Sensor& operator=(const Sensor&) = delete;
    virtual ~Sensor() = default;
    virtual std::string describe() const { return "generic sensor, value 0"; }
};

struct ThermoSensor : Sensor {
    explicit ThermoSensor(double t) : temp(t) {}
    std::string describe() const override {
        std::ostringstream os;
        os << "thermo, " << std::fixed << std::setprecision(1) << temp;
        return os.str();
    }
    double temp;
};

int main() {
    // Ako je vector<Sensor> (nije dobro): čuva samo Sensor delove kopija.
    // Treba ovako: kontejner pokazivača-vlasnika. Objekti ostaju celi, a
    // virtualni poziv ide u pravu klasu.
    std::vector<std::unique_ptr<Sensor>> sensors;
    sensors.push_back(std::make_unique<ThermoSensor>(21.5));
    sensors.push_back(std::make_unique<ThermoSensor>(30.0));
    for (const auto& s : sensors) std::cout << s->describe() << '\n';
}
