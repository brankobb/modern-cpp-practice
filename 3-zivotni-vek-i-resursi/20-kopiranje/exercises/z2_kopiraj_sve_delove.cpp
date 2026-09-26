// VRSTA: zašto
// DEMO-OUT: NAIVNO kopija: bez imena, kal 1.5
//
// Zadatak 2 -- zašto ručna kopija mora da kopira i baznu klasu
// (sekcija 5, EC++ Item 12)
// Rešenje: exercises/solutions/z2_kopiraj_sve_delove.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-zivotni-vek-i-resursi/20-kopiranje/exercises/z2_kopiraj_sve_delove.cpp -DNAIVNO
//   Senzor ima ručno napisan copy konstruktor (npr. da broji kopije) koji
//   kopira samo SVOJ član. Kopija dobije ime "bez imena": kad init lista
//   ne pomene baznu klasu, ona se pravi PODRAZUMEVANIM konstruktorom, ne
//   copy konstruktorom. Isto i dodela: baza se ne dodeli.
//   g++ -Wextra upozori ("base class ... should be explicitly initialized
//   in the copy constructor"), clang ćuti.
// Korak 2: u #else grani napiši Senzor ispravno: copy konstruktor pozove
//   Uredjaj(o) u init listi, a copy dodela pozove Uredjaj::operator=(o).
// Korak 3: kada ti ručna kopija uopšte ne treba? (Ovde samo zbog brojača
//   -- bez njega bi = default, ili ništa, bilo ispravno i kraće.)

#include <iostream>
#include <string>
#include <utility>

int kopija = 0;

struct Uredjaj {
    std::string ime;
    Uredjaj() : ime("bez imena") {}
    explicit Uredjaj(std::string i) : ime(std::move(i)) {}
};

#ifdef NAIVNO
struct Senzor : Uredjaj {
    double kal;
    Senzor(std::string i, double k) : Uredjaj(std::move(i)), kal(k) {}
    Senzor(const Senzor& o) : kal(o.kal) { ++kopija; }          // baza?
    Senzor& operator=(const Senzor& o) {
        kal = o.kal;                                             // baza?
        ++kopija;
        return *this;
    }
};

int main() {
    Senzor a("temperatura", 1.5);
    Senzor b(a);
    std::cout << "kopija: " << b.ime << ", kal " << b.kal << '\n';
    Senzor c("pritisak", 0.5);
    c = a;
    std::cout << "dodela: " << c.ime << ", kal " << c.kal << '\n';
}
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // Senzor a("temperatura", 1.5);
    // Senzor b(a);
    // std::cout << "kopija: " << b.ime << ", kal " << b.kal << '\n';
    // Senzor c("pritisak", 0.5);
    // c = a;
    // std::cout << "dodela: " << c.ime << ", kal " << c.kal << '\n';
    // std::cout << "kopiranja: " << kopija << '\n';
}
#endif

/* OČEKIVANI IZLAZ
kopija: temperatura, kal 1.5
dodela: temperatura, kal 1.5
kopiranja: 2
*/
