// VRSTA: upotreba
//
// Zadatak 1 -- static_cast, dynamic_cast i konverzija između tipova
// (sekcije 2, 4, 5, 7)
//   ./build.sh week0-fundamentals/14-type-conversions/exercises/z1_cast_dynamic.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_cast_dynamic.cpp
//
// Korak 1: double prosek(const std::vector<int>& v) -- zbir je int, a
//   deljenje mora biti u double: gde tačno ide static_cast? (Probaj bez
//   njega: 7 / 2 = 3.) Za prazan vektor vrati 0.
// Korak 2: hijerarhija Poruka (polimorfna: virtualni destruktor) ->
//   Tekst (std::string sadrzaj) i Komanda (int kod). Funkcija
//   void obradi(const Poruka& p): ako je p Komanda (dynamic_cast na
//   const Komanda*, pa provera na nullptr), ispiše "komanda <kod>", inače
//   "nije komanda".
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
    // std::cout << "prosek {3, 4}: " << prosek({3, 4}) << '\n';
    // std::cout << "prosek {}: " << prosek({}) << '\n';

    // Korak 2 -- otkomentariši:
    // Tekst t("zdravo");
    // Komanda k(7);
    // obradi(t);
    // obradi(k);

    // Korak 3 -- otkomentariši:
    // Celsius c(Fahrenheit{212});
    // std::cout << "212 F = " << static_cast<double>(c) << " C\n";
}

/* OČEKIVANI IZLAZ
prosek {3, 4}: 3.5
prosek {}: 0
nije komanda
komanda 7
212 F = 100 C
*/
