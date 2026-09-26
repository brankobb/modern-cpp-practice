// KIND: usage
//
// Zadatak 1 -- auto, range-for i structured bindings (sekcije 1, 3, 7, 8)
//   ./build.sh 1-language-basics/10-auto-and-range-for/exercises/ex1_structured_bindings.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_structured_bindings.cpp
//
// Korak 1: void print(const std::map<std::string, int>& s) -- range-for
//   sa structured binding-om: for (const auto& [name, quantity] : s).
//   Format: "name=quantity" razdvojeni sa ", ", pa novi red.
// Korak 2: void restock(std::map<std::string, int>& s, int amount) -- uveća
//   svaku količinu. Šta mora da se promeni u petlji da bi izmena ostala u
//   mapi?
// Korak 3: auto lowest(const std::map<std::string, int>& s) vraća par
//   (ime, količina) sa najmanjom količinom. Koristi std::min_element sa
//   lambdom koja prima (const auto& a, const auto& b) -- generička lambda,
//   C++14. Koji tip je auto za povratnu vrednost? (Hint: šta je
//   value_type mape?)
//   Zatim u main(): auto [name, qty] = lowest(stock);

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

// TODO korak 1, 2, 3

int main() {
    std::map<std::string, int> stock{{"resistor", 120}, {"capacitor", 45}, {"diode", 80}};
    (void)stock;

    // Korak 1 -- otkomentariši:
    // print(stock);

    // Korak 2 -- otkomentariši:
    // restock(stock, 10);
    // print(stock);

    // Korak 3 -- otkomentariši:
    // auto [name, qty] = lowest(stock);
    // std::cout << "lowest: " << name << " (" << qty << ")\n";
}

/* EXPECTED OUTPUT
capacitor=45, diode=80, resistor=120
capacitor=55, diode=90, resistor=130
lowest: capacitor (55)
*/
