// KIND: usage
//
// Zadatak 1 -- aritmetički operatori, poređenje i ispis (sekcije 2, 5, 6, 11)
//   ./build.sh 2-classes/15-operator-overloading/exercises/ex1_fraction.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_fraction.cpp
//
// Korak 1: class Fraction sa long num_ i den_. Konstruktor
//   Fraction(long n, long d = 1) baca std::invalid_argument za d == 0 i
//   NORMALIZUJE: skrati sa std::gcd (<numeric>, C++17), a znak drži u
//   brojiocu (imenilac uvek > 0). Konstruktor namerno NIJE explicit: 2 kao
//   Fraction(2) je prirodna konverzija (izuzetak iz C.46).
// Korak 2: operator+= i operator*= kao članovi (menjaju *this, vraćaju
//   Fraction&), a operator+ i operator* kao SLOBODNE funkcije napisane
//   preko njih. Zašto slobodne? Probaj 2 + a. Unarni operator- (član, const).
// Korak 3: operator== i operator< (slobodne; poređenje unakrsnim
//   množenjem), i operator<< koji ispiše "n/d", ili samo "n" kad je d == 1.

#include <iostream>
#include <numeric>
#include <stdexcept>

class Fraction {
public:
    // TODO korak 1, 2
};

// TODO korak 2, 3 (slobodne funkcije)

int main() {
    // Korak 1-3 -- otkomentariši:
    // Fraction a(1, 2), b(3, 4);
    // std::cout << "a + b = " << a + b << '\n';
    // std::cout << "2 + a = " << 2 + a << '\n';
    // std::cout << "a * b = " << a * b << '\n';
    // std::cout << "-a = " << -a << '\n';
    // std::cout << "Fraction(2, 4) == a: " << (Fraction(2, 4) == a) << '\n';
    // std::cout << "a < b: " << (a < b) << '\n';
    // std::cout << "Fraction(6, -3) = " << Fraction(6, -3) << '\n';
    // try {
    //     Fraction bad(1, 0);
    // } catch (const std::invalid_argument&) {
    //     std::cout << "denominator 0: rejected\n";
    // }
}

/* EXPECTED OUTPUT
a + b = 5/4
2 + a = 5/2
a * b = 3/8
-a = -1/2
Fraction(2, 4) == a: 1
a < b: 1
Fraction(6, -3) = -2
denominator 0: rejected
*/
