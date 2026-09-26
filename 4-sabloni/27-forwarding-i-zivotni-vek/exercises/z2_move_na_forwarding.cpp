// VRSTA: zašto
// DEMO-OUT: NAIVNO ime pozivaoca posle: ""
//
// Zadatak 2 -- zašto std::forward, a ne std::move, na forwarding referenci
// (sekcije 1, 3, EMC Item 25)
// Rešenje: exercises/solutions/z2_move_na_forwarding.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-sabloni/27-forwarding-i-zivotni-vek/exercises/z2_move_na_forwarding.cpp -DNAIVNO
//   Pozivalac je prosledio svoju promenljivu (lvalue) i posle je koristi --
//   a ona je prazna. T&& u template-u prima i lvalue, a std::move
//   BEZUSLOVNO pravi rvalue, pa se tuđi objekat pomeri. Bez upozorenja.
// Korak 2: u #else grani napiši postaviIme sa std::forward<T>(ime):
//   rvalue se pomeri, lvalue se kopira. Pravilo (EMC Item 25): std::move
//   na rvalue referenci (T&& gde T nije template parametar),
//   std::forward na forwarding referenci.

#include <iostream>
#include <string>
#include <utility>

struct Uredjaj {
    std::string ime;
#ifdef NAIVNO
    template <typename T>
    void postaviIme(T&& novo) { ime = std::move(novo); }    // pomeri i lvalue!
#else
    // TODO korak 2 (dok ne napišeš, ova verzija uvek kopira)
    template <typename T>
    void postaviIme(const T& novo) { ime = novo; }
#endif
};

int main() {
    Uredjaj u;
    std::string ime = "senzor-temperature-01";
    u.postaviIme(ime);
    std::cout << "uređaj: " << u.ime << '\n';
    std::cout << "ime pozivaoca posle: \"" << ime << "\"\n";
    u.postaviIme(std::string("senzor-pritiska-02"));
    std::cout << "uređaj: " << u.ime << '\n';
}

/* OČEKIVANI IZLAZ
uređaj: senzor-temperature-01
ime pozivaoca posle: "senzor-temperature-01"
uređaj: senzor-pritiska-02
*/
