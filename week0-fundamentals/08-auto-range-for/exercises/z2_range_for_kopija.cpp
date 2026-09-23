// VRSTA: zašto
// DEMO-OUT: NAIVNO kopija: 3, s\[0\]\.temp = 70
//
// Zadatak 2 -- zašto "auto&" / "const auto&" u range-for (sekcija 7)
// Rešenje: exercises/solutions/z2_range_for_kopija.cpp
//
// Senzor broji koliko puta je kopiran.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week0-fundamentals/08-auto-range-for/exercises/z2_range_for_kopija.cpp -DNAIVNO
//   Petlja "for (auto s : senzori) s.temp = 0;" napravi 3 kopije i
//   resetuje KOPIJE -- original je i dalje 70. Kompajler ne upozori.
//   auto dedukuje tip kao za parametar po vrednosti (EMC Item 2):
//   referenca se odbacuje.
// Korak 2: u #else grani napiši reset(): petlja koja menja -> auto&.
// Korak 3: napiši double prosek(const std::vector<Senzor>&): petlja koja
//   samo čita -> const auto&. Proveri da brojač kopija ostaje 0.

#include <iostream>
#include <vector>

int kopija = 0;

struct Senzor {
    double temp;
    Senzor(double t) : temp(t) {}
    Senzor(const Senzor& o) : temp(o.temp) { ++kopija; }
    Senzor& operator=(const Senzor&) = default;
};

int main() {
    std::vector<Senzor> senzori{70.0, 80.0, 90.0};
    kopija = 0;   // initializer_list kopira pri pravljenju vektora; to ne brojimo
#ifdef NAIVNO
    for (auto s : senzori) s.temp = 0;
    std::cout << "kopija: " << kopija << ", s[0].temp = " << senzori[0].temp << '\n';
#else
    // TODO korak 2 i 3
    // Korak 2 i 3 -- otkomentariši:
    // std::cout << "prosek pre: " << prosek(senzori) << '\n';
    // reset(senzori);
    // std::cout << "kopija: " << kopija << ", s[0].temp = " << senzori[0].temp << '\n';
    // std::cout << "prosek posle: " << prosek(senzori) << '\n';
#endif
}

/* OČEKIVANI IZLAZ
prosek pre: 80
kopija: 0, s[0].temp = 0
prosek posle: 0
*/
