// EXPECT-UB: pure virtual method called
// POGREŠNO: konstruktor baze (posredno) poziva pure virtual funkciju.
// Zašto: dok se pravi Base deo, objekat JE Base (EC++ Item 9); Derived deo
//   još ne postoji. Virtual poziv zato ide na Base::setup, a ona je "= 0".
//   Direktan poziv setup() iz konstruktora oba kompajlera prijave
//   (upozorenje, a g++ ga ni ne linkuje), ali kroz pomoćnu funkciju init()
//   ne vidi nijedan. Runtime prekine program porukom
//   "pure virtual method called".
// Ispravno: ne zovi virtual funkcije iz konstruktora i destruktora. Ako
//   izvedena klasa mora nešto da podesi, neka to uradi u svom konstruktoru,
//   ili neka prosledi podatke konstruktoru baze.
#include <cstdio>

class Base {
public:
    Base() { init(); }
    virtual ~Base() = default;
    void init() { setup(); }
    virtual void setup() = 0;
};

class Derived : public Base {
public:
    void setup() override { std::puts("Derived::setup"); }
};

int main() {
    Derived d;
}
