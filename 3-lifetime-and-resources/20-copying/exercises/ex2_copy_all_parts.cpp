// KIND: why
// DEMO-OUT: NAIVE copy: unnamed, cal 1.5
//
// Zadatak 2 -- zašto ručna kopija mora da kopira i baznu klasu
// (sekcija 5, EC++ Item 12)
// Rešenje: exercises/solutions/ex2_copy_all_parts.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/20-copying/exercises/ex2_copy_all_parts.cpp -DNAIVE
//   Sensor ima ručno napisan copy konstruktor (npr. da broji kopije) koji
//   kopira samo SVOJ član. Kopija dobije ime "unnamed": kad init lista
//   ne pomene baznu klasu, ona se pravi PODRAZUMEVANIM konstruktorom, ne
//   copy konstruktorom. Isto i dodela: baza se ne dodeli.
//   g++ -Wextra upozori ("base class ... should be explicitly initialized
//   in the copy constructor"), clang ćuti.
// Korak 2: u #else grani napiši Sensor ispravno: copy konstruktor pozove
//   Device(o) u init listi, a copy dodela pozove Device::operator=(o).
// Korak 3: kada ti ručna kopija uopšte ne treba? (Ovde samo zbog brojača
//   -- bez njega bi = default, ili ništa, bilo ispravno i kraće.)

#include <iostream>
#include <string>
#include <utility>

int copies = 0;

struct Device {
    std::string name;
    Device() : name("unnamed") {}
    explicit Device(std::string n) : name(std::move(n)) {}
};

#ifdef NAIVE
struct Sensor : Device {
    double cal;
    Sensor(std::string n, double c) : Device(std::move(n)), cal(c) {}
    Sensor(const Sensor& o) : cal(o.cal) { ++copies; }           // baza?
    Sensor& operator=(const Sensor& o) {
        cal = o.cal;                                             // baza?
        ++copies;
        return *this;
    }
};

int main() {
    Sensor a("temperature", 1.5);
    Sensor b(a);
    std::cout << "copy: " << b.name << ", cal " << b.cal << '\n';
    Sensor c("pressure", 0.5);
    c = a;
    std::cout << "assign: " << c.name << ", cal " << c.cal << '\n';
}
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // Sensor a("temperature", 1.5);
    // Sensor b(a);
    // std::cout << "copy: " << b.name << ", cal " << b.cal << '\n';
    // Sensor c("pressure", 0.5);
    // c = a;
    // std::cout << "assign: " << c.name << ", cal " << c.cal << '\n';
    // std::cout << "copies: " << copies << '\n';
}
#endif

/* EXPECTED OUTPUT
copy: temperature, cal 1.5
assign: temperature, cal 1.5
copies: 2
*/
