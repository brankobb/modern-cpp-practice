// KIND: usage
//
// Zadatak 1 -- const metode, mutable keš i const/non-const par (sekcije 4, 5, 7)
//   ./build.sh 1-language-basics/09-const/exercises/ex1_const_methods.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_const_methods.cpp
//
// Klasa Readings čuva niz temperatura (std::vector<double>).
// Korak 1: void add(double t) i std::size_t count() const, double
//   last() const. Koje metode moraju biti const da bi
//   report(const Readings&) radio?
// Korak 2: double average() const sa KEŠOM: prosek se računa samo kad je
//   niz promenjen od prošlog računanja. Keš (vrednost, "važi" i brojač
//   računanja) su mutable članovi -- logički ne menjaju objekat
//   (EC++ Item 3: "logical constness"). add() poništava keš.
//   int computations() const vraća koliko puta je prosek stvarno računat.
// Korak 3: par operatora
//       const double& operator[](std::size_t i) const;
//       double& operator[](std::size_t i);
//   Napiši const verziju, a non-const napravi pozivom const verzije i
//   const_cast-om na rezultatu (EC++ Item 3: bez dupliranja koda).
//   Pazi: izmena kroz [] mora da poništi keš.

#include <cstddef>
#include <iostream>
#include <vector>

class Readings {
public:
    // TODO korak 1, 2, 3
};

// void report(const Readings& r) {
//     std::cout << "count: " << r.count() << ", last: " << r.last()
//               << ", average: " << r.average() << " (computations: " << r.computations() << ")\n";
// }

int main() {
    // Korak 1 i 2 -- otkomentariši (i report iznad):
    // Readings r;
    // r.add(20.0);
    // r.add(22.0);
    // report(r);
    // report(r);            // keš: bez novog računanja
    // r.add(26.0);
    // report(r);

    // Korak 3 -- otkomentariši:
    // r[0] = 29.0;
    // report(r);
    // const Readings& cr = r;
    // std::cout << "cr[1] = " << cr[1] << '\n';
}

/* EXPECTED OUTPUT
count: 2, last: 22, average: 21 (computations: 1)
count: 2, last: 22, average: 21 (computations: 1)
count: 3, last: 26, average: 22.6667 (computations: 2)
count: 3, last: 26, average: 25.6667 (computations: 3)
cr[1] = 22
*/
