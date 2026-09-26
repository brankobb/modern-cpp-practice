// Rešenje zadatka ex2_copy_all_parts.

#include <iostream>
#include <string>
#include <utility>

int copies = 0;

struct Device {
    std::string name;
    Device() : name("unnamed") {}
    explicit Device(std::string n) : name(std::move(n)) {}
};

// Ako ručna kopija izostavi baznu klasu (nije dobro): copy konstruktor
// napravi bazu podrazumevanim konstruktorom, a dodela je ne dira -- kopija
// tiho izgubi deo stanja.
// Treba ovako: u init listi copy konstruktora Device(o), a u dodeli
// Device::operator=(o). (Sensor& se implicitno konvertuje u Device&.)
struct Sensor : Device {
    double cal;
    Sensor(std::string n, double c) : Device(std::move(n)), cal(c) {}
    Sensor(const Sensor& o) : Device(o), cal(o.cal) { ++copies; }
    Sensor& operator=(const Sensor& o) {
        Device::operator=(o);
        cal = o.cal;
        ++copies;
        return *this;
    }
};

// Korak 3: bez brojača ne piši ništa -- generisane kopije kopiraju SVE
// (baze i sve članove), i ne mogu da zaborave novi član dodat kasnije.

int main() {
    Sensor a("temperature", 1.5);
    Sensor b(a);
    std::cout << "copy: " << b.name << ", cal " << b.cal << '\n';
    Sensor c("pressure", 0.5);
    c = a;
    std::cout << "assign: " << c.name << ", cal " << c.cal << '\n';
    std::cout << "copies: " << copies << '\n';
}
