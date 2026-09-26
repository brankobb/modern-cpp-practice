// KIND: usage
//
// Zadatak 1 -- auto, range-for i structured bindings (sekcije 1, 3, 7, 8)
//   ./build.sh 1-language-basics/10-auto-and-range-for/exercises/ex1_structured_bindings.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_structured_bindings.cpp
//
// Korak 1: void ispisi(const std::map<std::string, int>& z) -- range-for
//   sa structured binding-om: for (const auto& [ime, kolicina] : z).
//   Format: "ime=kolicina" razdvojeni sa ", ", pa novi red.
// Korak 2: void dopuni(std::map<std::string, int>& z, int koliko) -- uveća
//   svaku količinu. Šta mora da se promeni u petlji da bi izmena ostala u
//   mapi?
// Korak 3: auto najmanje(const std::map<std::string, int>& z) vraća par
//   (ime, količina) sa najmanjom količinom. Koristi std::min_element sa
//   lambdom koja prima (const auto& a, const auto& b) -- generička lambda,
//   C++14. Koji tip je auto za povratnu vrednost? (Hint: šta je
//   value_type mape?)
//   Zatim u main(): auto [ime, kol] = najmanje(zalihe);

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

// TODO korak 1, 2, 3

int main() {
    std::map<std::string, int> zalihe{{"otpornik", 120}, {"kondenzator", 45}, {"dioda", 80}};
    (void)zalihe;

    // Korak 1 -- otkomentariši:
    // ispisi(zalihe);

    // Korak 2 -- otkomentariši:
    // dopuni(zalihe, 10);
    // ispisi(zalihe);

    // Korak 3 -- otkomentariši:
    // auto [ime, kol] = najmanje(zalihe);
    // std::cout << "najmanje: " << ime << " (" << kol << ")\n";
}

/* EXPECTED OUTPUT
dioda=80, kondenzator=45, otpornik=120
dioda=90, kondenzator=55, otpornik=130
najmanje: kondenzator (55)
*/
