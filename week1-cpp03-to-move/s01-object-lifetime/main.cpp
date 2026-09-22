#include <iostream>

// Vežba: napravi hijerarhiju Base -> Derived sa 2-3 člana svaki
// (neki po vrednosti, neki dinamički alocirani). Ispiši iz ctor/dtor
// redosled konstrukcije/destrukcije i proveri da odgovara očekivanju:
//   baza -> članovi po redosledu deklaracije -> telo ctor-a
//   dtor obrnutim redosledom
//
// Probaj i:
//  - objekat na stack-u (automatic storage)
//  - static lokalni objekat u funkciji
//  - new/delete (dynamic storage)
//  - namerno pomešaj redosled u init listi vs redosled deklaracije
//    i vidi upozorenje kompajlera (-Wreorder / -Wall)
//
// Ako u init listi napišeš članove DRUGAČIJIM redosledom nego što su
// DEKLARISANI u klasi (NIJE DOBRO, zbunjujuće) jer se oni SVEJEDNO
// konstruišu redosledom DEKLARACIJE, ne redosledom napisanim u init
// listi -- kompajler te upozori (-Wreorder) ali ne spreči.
// Treba da init lista PRATI redosled deklaracije -- tako kod čitaš i
// izvršava se u istom redosledu, bez iznenađenja (posebno bitno ako
// jedan član zavisi od vrednosti drugog već konstruisanog člana).
// Možeš i preurediti REDOSLED DEKLARACIJE u klasi ako ti init lista
// prirodnije ide drugačijim redosledom -- ali onda menjaš dizajn klase,
// ne samo init listu.

struct Base {
    Base() { std::cout << "Base ctor\n"; }
    ~Base() { std::cout << "Base dtor\n"; }
};

struct Derived : Base {
    Derived() { std::cout << "Derived ctor\n"; }
    ~Derived() { std::cout << "Derived dtor\n"; }
};

int main() {
    Derived d;
}
