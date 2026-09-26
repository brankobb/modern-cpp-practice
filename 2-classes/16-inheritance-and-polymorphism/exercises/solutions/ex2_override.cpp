// Rešenje zadatka ex2_override.

#include <iostream>

struct Sensor {
    virtual ~Sensor() = default;
    virtual double read() const {
        std::cout << "Sensor::read -- default\n";
        return 0.0;
    }
};

// Ako izostaviš const (ili promeniš tip parametra) bez override (nije
// dobro): nastane nova funkcija, a virtualni poziv tiho ide u baznu klasu.
// Treba ovako: override na SVAKOJ funkciji koja nadjačava (C.128). Potpis
// koji se ne poklapa postaje greška pri kompajliranju. virtual se u
// izvedenoj klasi ne piše -- override ga podrazumeva.
struct ThermoSensor : Sensor {
    double read() const override {
        std::cout << "ThermoSensor::read\n";
        return 21.5;
    }
};

void report(const Sensor& s) {
    double v = s.read();   // prvo očitaj (read() i sam ispisuje), pa ispiši
    std::cout << "value: " << v << '\n';
}

int main() {
    ThermoSensor t;
    report(t);
}
