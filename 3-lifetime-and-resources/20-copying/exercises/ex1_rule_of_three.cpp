// KIND: usage
//
// Zadatak 1 -- rule of 3: duboka kopija, copy-and-swap, destruktor (sekcija 4)
//   ./build.sh 3-lifetime-and-resources/20-copying/exercises/ex1_rule_of_three.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_rule_of_three.cpp
//
// class Text drži svoj char niz na heap-u: char* data_ i
// std::size_t length_ (bez '\0' u dužini).
// Korak 1: explicit Text(const char* s) -- alociraj length_ + 1 i kopiraj
//   (std::strlen, std::memcpy iz <cstring>). Destruktor: delete[].
//   const char* c_str() const, std::size_t length() const.
// Korak 2: copy konstruktor -- DUBOKA kopija (novi blok + kopiranje).
//   void set(std::size_t i, char c) menja jedan znak, da bi se videlo
//   da su kopije nezavisne.
// Korak 3: copy dodela idiomom copy-and-swap:
//       Text& operator=(const Text& o) { Text copy(o); swap(copy); return *this; }
//   uz void swap(Text& o) noexcept koji zameni pokazivače i dužine.
//   Radi i za a = a, i ako alokacija baci -- *this ostaje netaknut.

#include <cstddef>
#include <cstring>
#include <iostream>
#include <utility>

class Text {
public:
    // TODO korak 1, 2, 3
};

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // Text a("motor");
    // Text b(a);
    // b.set(0, 'M');
    // std::cout << "a: " << a.c_str() << ", b: " << b.c_str() << ", length " << b.length() << '\n';

    // Korak 3 -- otkomentariši:
    // Text c("x");
    // c = a;
    // a.set(4, 'R');
    // std::cout << "a: " << a.c_str() << ", c: " << c.c_str() << '\n';
    // Text& same = c;
    // c = same;                                   // dodela samom sebi
    // std::cout << "after c = c: " << c.c_str() << '\n';
    // Text d("d"), e("e");
    // d = e = b;                                  // lanac: vraća Text&
    // std::cout << "d: " << d.c_str() << ", e: " << e.c_str() << '\n';
}

/* EXPECTED OUTPUT
a: motor, b: Motor, length 5
a: motoR, c: motor
after c = c: motor
d: Motor, e: Motor
*/
