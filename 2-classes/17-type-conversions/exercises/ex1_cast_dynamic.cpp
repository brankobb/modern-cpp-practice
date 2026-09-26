// KIND: usage
//
// Zadatak 1 -- static_cast, dynamic_cast i konverzija između tipova
// (sekcije 2, 4, 5, 7)
//   ./build.sh 2-classes/17-type-conversions/exercises/ex1_cast_dynamic.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_cast_dynamic.cpp
//
// Korak 1: double average(const std::vector<int>& v) -- zbir je int, a
//   deljenje mora biti u double: gde tačno ide static_cast? (Probaj bez
//   njega: 7 / 2 = 3.) Za prazan vektor vrati 0.
// Korak 2: hijerarhija Message (polimorfna: virtualni destruktor) ->
//   Text (std::string content) i Command (int code). Funkcija
//   void handle(const Message& m): ako je m Command (dynamic_cast na
//   const Command*, pa provera na nullptr), ispiše "command <code>", inače
//   "not a command".
// Korak 3: struct Fahrenheit { double f; }; i class Celsius sa
//   explicit Celsius(Fahrenheit) (konverzija u ODREDIŠNOJ klasi, jedno
//   mesto -- sekcija 7) i explicit operator double() const.
//   static_cast<double>(Celsius(Fahrenheit{212})) daje 100.

#include <iostream>
#include <string>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::cout << "average {3, 4}: " << average({3, 4}) << '\n';
    // std::cout << "average {}: " << average({}) << '\n';

    // Korak 2 -- otkomentariši:
    // Text t("hello");
    // Command c(7);
    // handle(t);
    // handle(c);

    // Korak 3 -- otkomentariši:
    // Celsius deg(Fahrenheit{212});
    // std::cout << "212 F = " << static_cast<double>(deg) << " C\n";
}

/* EXPECTED OUTPUT
average {3, 4}: 3.5
average {}: 0
not a command
command 7
212 F = 100 C
*/
