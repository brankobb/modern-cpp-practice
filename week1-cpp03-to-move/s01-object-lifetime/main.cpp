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
