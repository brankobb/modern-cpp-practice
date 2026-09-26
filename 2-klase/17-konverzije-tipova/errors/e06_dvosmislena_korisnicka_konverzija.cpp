// STD: c++17
// EXPECT-GCC: conversion from 'Fahrenheit' to 'Celsius' is ambiguous
// EXPECT-CLANG: conversion from 'Fahrenheit' to 'Celsius' is ambiguous
// POGREŠNO: ista konverzija napisana dvaput: konstruktor Celsius(const
//   Fahrenheit&) I operator Fahrenheit::operator Celsius().
// Zašto: kod Celsius c = f; (copy inicijalizacija) oba puta su jednako
//   dobra korisnička konverzija, pa kompajler ne može da izabere.
//   Celsius c(f); bi se kompajlirao (direct-init bira konstruktor, test:
//   g++ i clang), ali greška čeka prvu dodelu ili poziv funkcije.
// Ispravno: konverzija na JEDNOM mestu, najbolje konstruktor u odredišnoj
//   klasi (main.cpp, sekcija 7).
struct Celsius;

struct Fahrenheit {
    double v;
    operator Celsius() const;
};

struct Celsius {
    double v;
    Celsius(double x) : v(x) {}
    Celsius(const Fahrenheit& f) : v((f.v - 32) / 1.8) {}
};

Fahrenheit::operator Celsius() const { return Celsius((v - 32) / 1.8); }

int main() {
    Fahrenheit f{212};
    Celsius c = f;
    return static_cast<int>(c.v);
}
