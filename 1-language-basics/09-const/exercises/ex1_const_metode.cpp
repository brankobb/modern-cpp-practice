// KIND: usage
//
// Zadatak 1 -- const metode, mutable keš i const/non-const par (sekcije 4, 5, 7)
//   ./build.sh 1-language-basics/09-const/exercises/ex1_const_metode.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_const_metode.cpp
//
// Klasa Merenja čuva niz temperatura (std::vector<double>).
// Korak 1: void dodaj(double t) i std::size_t broj() const, double
//   poslednja() const. Koje metode moraju biti const da bi
//   izvestaj(const Merenja&) radio?
// Korak 2: double prosek() const sa KEŠOM: prosek se računa samo kad je
//   niz promenjen od prošlog računanja. Keš (vrednost, "važi" i brojač
//   računanja) su mutable članovi -- logički ne menjaju objekat
//   (EC++ Item 3: "logical constness"). dodaj() poništava keš.
//   int racunanja() const vraća koliko puta je prosek stvarno računat.
// Korak 3: par operatora
//       const double& operator[](std::size_t i) const;
//       double& operator[](std::size_t i);
//   Napiši const verziju, a non-const napravi pozivom const verzije i
//   const_cast-om na rezultatu (EC++ Item 3: bez dupliranja koda).
//   Pazi: izmena kroz [] mora da poništi keš.

#include <cstddef>
#include <iostream>
#include <vector>

class Merenja {
public:
    // TODO korak 1, 2, 3
};

// void izvestaj(const Merenja& m) {
//     std::cout << "broj: " << m.broj() << ", poslednja: " << m.poslednja()
//               << ", prosek: " << m.prosek() << " (računanja: " << m.racunanja() << ")\n";
// }

int main() {
    // Korak 1 i 2 -- otkomentariši (i izvestaj iznad):
    // Merenja m;
    // m.dodaj(20.0);
    // m.dodaj(22.0);
    // izvestaj(m);
    // izvestaj(m);          // keš: bez novog računanja
    // m.dodaj(26.0);
    // izvestaj(m);

    // Korak 3 -- otkomentariši:
    // m[0] = 29.0;
    // izvestaj(m);
    // const Merenja& cm = m;
    // std::cout << "cm[1] = " << cm[1] << '\n';
}

/* EXPECTED OUTPUT
broj: 2, poslednja: 22, prosek: 21 (računanja: 1)
broj: 2, poslednja: 22, prosek: 21 (računanja: 1)
broj: 3, poslednja: 26, prosek: 22.6667 (računanja: 2)
broj: 3, poslednja: 26, prosek: 25.6667 (računanja: 3)
cm[1] = 22
*/
