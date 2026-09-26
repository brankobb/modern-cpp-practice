// KIND: why
// DEMO-OUT: NAIVE copies: 3, s\[0\]\.temp = 70
//
// Zadatak 2 -- zašto "auto&" / "const auto&" u range-for (sekcija 7)
// Rešenje: exercises/solutions/ex2_range_for_copy.cpp
//
// Sensor broji koliko puta je kopiran.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/10-auto-and-range-for/exercises/ex2_range_for_copy.cpp -DNAIVE
//   Petlja "for (auto s : sensors) s.temp = 0;" napravi 3 kopije i
//   resetuje KOPIJE -- original je i dalje 70. Kompajler ne upozori.
//   auto dedukuje tip kao za parametar po vrednosti (EMC Item 2):
//   referenca se odbacuje.
// Korak 2: u #else grani napiši reset(): petlja koja menja -> auto&.
// Korak 3: napiši double average(const std::vector<Sensor>&): petlja koja
//   samo čita -> const auto&. Proveri da brojač kopija ostaje 0.

#include <iostream>
#include <vector>

int copies = 0;

struct Sensor {
    double temp;
    Sensor(double t) : temp(t) {}
    Sensor(const Sensor& o) : temp(o.temp) { ++copies; }
    Sensor& operator=(const Sensor&) = default;
};

int main() {
    std::vector<Sensor> sensors{70.0, 80.0, 90.0};
    copies = 0;   // initializer_list kopira pri pravljenju vektora; to ne brojimo
#ifdef NAIVE
    for (auto s : sensors) s.temp = 0;
    std::cout << "copies: " << copies << ", s[0].temp = " << sensors[0].temp << '\n';
#else
    // TODO korak 2 i 3
    // Korak 2 i 3 -- otkomentariši:
    // std::cout << "average before: " << average(sensors) << '\n';
    // reset(sensors);
    // std::cout << "copies: " << copies << ", s[0].temp = " << sensors[0].temp << '\n';
    // std::cout << "average after: " << average(sensors) << '\n';
#endif
}

/* EXPECTED OUTPUT
average before: 80
copies: 0, s[0].temp = 0
average after: 0
*/
