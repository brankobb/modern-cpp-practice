// VRSTA: upotreba
//
// Zadatak 1 -- rule of 3: duboka kopija, copy-and-swap, destruktor (sekcija 4)
//   ./build.sh week1-cpp03-to-move/s02-copying/exercises/z1_rule_of_three.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_rule_of_three.cpp
//
// class Tekst drži svoj char niz na heap-u: char* podaci_ i
// std::size_t duzina_ (bez '\0' u dužini).
// Korak 1: explicit Tekst(const char* s) -- alociraj duzina_ + 1 i kopiraj
//   (std::strlen, std::memcpy iz <cstring>). Destruktor: delete[].
//   const char* c_str() const, std::size_t duzina() const.
// Korak 2: copy konstruktor -- DUBOKA kopija (novi blok + kopiranje).
//   void promeni(std::size_t i, char c) menja jedan znak, da bi se videlo
//   da su kopije nezavisne.
// Korak 3: copy dodela idiomom copy-and-swap:
//       Tekst& operator=(const Tekst& o) { Tekst kopija(o); swap(kopija); return *this; }
//   uz void swap(Tekst& o) noexcept koji zameni pokazivače i dužine.
//   Radi i za a = a, i ako alokacija baci -- *this ostaje netaknut.

#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>

class Tekst {
public:
    // TODO korak 1, 2, 3
};

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // Tekst a("motor");
    // Tekst b(a);
    // b.promeni(0, 'M');
    // std::cout << "a: " << a.c_str() << ", b: " << b.c_str() << ", dužina " << b.duzina() << '\n';

    // Korak 3 -- otkomentariši:
    // Tekst c("x");
    // c = a;
    // a.promeni(4, 'R');
    // std::cout << "a: " << a.c_str() << ", c: " << c.c_str() << '\n';
    // Tekst& isti = c;
    // c = isti;                                   // dodela samom sebi
    // std::cout << "posle c = c: " << c.c_str() << '\n';
    // Tekst d("d"), e("e");
    // d = e = b;                                  // lanac: vraća Tekst&
    // std::cout << "d: " << d.c_str() << ", e: " << e.c_str() << '\n';
}

/* OČEKIVANI IZLAZ
a: motor, b: Motor, dužina 5
a: motoR, c: motor
posle c = c: motor
d: Motor, e: Motor
*/
