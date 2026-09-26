// KIND: why
// DEMO-OUT: NAIVE caller's name after: ""
//
// Zadatak 2 -- zašto std::forward, a ne std::move, na forwarding referenci
// (sekcije 1, 3, EMC Item 25)
// Rešenje: exercises/solutions/ex2_move_on_forwarding.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/27-forwarding-and-lifetime/exercises/ex2_move_on_forwarding.cpp -DNAIVE
//   Pozivalac je prosledio svoju promenljivu (lvalue) i posle je koristi --
//   a ona je prazna. T&& u template-u prima i lvalue, a std::move
//   BEZUSLOVNO pravi rvalue, pa se tuđi objekat pomeri. Bez upozorenja.
// Korak 2: u #else grani napiši setName sa std::forward<T>(newName):
//   rvalue se pomeri, lvalue se kopira. Pravilo (EMC Item 25): std::move
//   na rvalue referenci (T&& gde T nije template parametar),
//   std::forward na forwarding referenci.

#include <iostream>
#include <string>
#include <utility>

struct Device {
    std::string name;
#ifdef NAIVE
    template <typename T>
    void setName(T&& newName) { name = std::move(newName); }    // pomeri i lvalue!
#else
    // TODO korak 2 (dok ne napišeš, ova verzija uvek kopira)
    template <typename T>
    void setName(const T& newName) { name = newName; }
#endif
};

int main() {
    Device u;
    std::string name = "temperature-sensor-01";
    u.setName(name);
    std::cout << "device: " << u.name << '\n';
    std::cout << "caller's name after: \"" << name << "\"\n";
    u.setName(std::string("pressure-sensor-02"));
    std::cout << "device: " << u.name << '\n';
}

/* EXPECTED OUTPUT
device: temperature-sensor-01
caller's name after: "temperature-sensor-01"
device: pressure-sensor-02
*/
