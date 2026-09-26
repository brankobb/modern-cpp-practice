// KIND: why
// DEMO-OUT: NAIVNO vrata B: ZATVORENA
//
// Zadatak 2 -- zašto if (o) nije isto što i if (*o) za optional<bool> (sekcija 3)
// Rešenje: exercises/solutions/ex2_optional_bool.cpp
//
// Senzor vrata javlja true (zatvorena), false (otvorena) ili ne odgovara
// (nullopt). Za troja vrata: A zatvorena, B otvorena, C ne odgovara.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 7-standard-library/37-optional-variant-any/exercises/ex2_optional_bool.cpp -DNAIVNO
//   Vrata B su prijavljena kao ZATVORENA, a otvorena su.
//   if (o) pita "IMA LI vrednost", ne "da li je vrednost true". Za
//   optional<bool> = false odgovor je "ima" -- true. Isto važi za
//   optional<int> = 0 i optional<T*> = nullptr. Kompajler ne upozori.
// Korak 2: u #else grani napiši opis() tako da razlikuje sva tri stanja:
//   nema vrednosti -> "NEPOZNATO", *o == true -> "ZATVORENA", inače
//   "OTVORENA". (Eksplicitno: o.has_value(), o == true, o.value_or(...).)

#include <iostream>
#include <optional>
#include <string>

std::optional<bool> ocitaj(char vrata) {
    if (vrata == 'A') return true;
    if (vrata == 'B') return false;
    return std::nullopt;
}

#ifdef NAIVNO
std::string opis(std::optional<bool> o) {
    if (o) return "ZATVORENA";
    if (!o.has_value()) return "NEPOZNATO";
    return "OTVORENA";
}
#else
// TODO korak 2 (dok ne napišeš, sve je "?")
std::string opis(std::optional<bool>) { return "?"; }
#endif

int main() {
    for (char v : {'A', 'B', 'C'}) std::cout << "vrata " << v << ": " << opis(ocitaj(v)) << '\n';
}

/* EXPECTED OUTPUT
vrata A: ZATVORENA
vrata B: OTVORENA
vrata C: NEPOZNATO
*/
