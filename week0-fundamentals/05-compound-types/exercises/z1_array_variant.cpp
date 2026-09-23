// VRSTA: upotreba
//
// Zadatak 1 -- std::array, std::variant i alias za pokazivač na funkciju
// (sekcije 3, 6, 7, 8)
//   ./build.sh week0-fundamentals/05-compound-types/exercises/z1_array_variant.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_array_variant.cpp
//
// Korak 1: double prosek(const std::array<int, 5>& a) -- prosek elemenata.
//   U testu: size(), front(), back(), i at(5) koji baca
//   std::out_of_range (za razliku od [5], koji je UB).
// Korak 2: using Poruka = std::variant<int, double, std::string>;
//   void opisi(const Poruka& p) ispisuje "int 42" / "double 3.5" /
//   "string zdravo". Koristi std::get_if (vraća nullptr kad variant ne
//   drži taj tip).
// Korak 3: using Obrada = int (*)(int); -- alias za pokazivač na funkciju.
//   Napiši int udvostruci(int) i int dodajJedan(int), i
//   int primeni(const std::array<Obrada, 2>& koraci, int x) koja redom
//   primeni korake.

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::array<int, 5> ocitavanja{10, 20, 30, 40, 50};
    // std::cout << "broj: " << ocitavanja.size() << " prvi: " << ocitavanja.front()
    //           << " poslednji: " << ocitavanja.back() << '\n';
    // std::cout << "prosek: " << prosek(ocitavanja) << '\n';
    // try {
    //     std::cout << ocitavanja.at(5);
    // } catch (const std::out_of_range&) {
    //     std::cout << "at(5): out_of_range\n";
    // }

    // Korak 2 -- otkomentariši:
    // for (const Poruka& p : {Poruka{42}, Poruka{3.5}, Poruka{std::string("zdravo")}})
    //     opisi(p);

    // Korak 3 -- otkomentariši:
    // std::array<Obrada, 2> koraci{udvostruci, dodajJedan};
    // std::cout << "10 -> " << primeni(koraci, 10) << '\n';
}

/* OČEKIVANI IZLAZ
broj: 5 prvi: 10 poslednji: 50
prosek: 30
at(5): out_of_range
int 42
double 3.5
string zdravo
10 -> 21
*/
