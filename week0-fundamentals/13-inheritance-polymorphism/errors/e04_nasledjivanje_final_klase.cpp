// STD: c++17
// EXPECT-GCC: cannot derive from 'final' base 'Base' in derived type 'Derived'
// EXPECT-CLANG: base 'Base' is marked 'final'
// POGREŠNO: nasleđivanje klase označene sa final.
// Zašto: final na klasi kaže da hijerarhija tu završava. Koristi se kad
//   klasa nije napravljena za nasleđivanje (npr. nema virtual destruktor),
//   a kompajler tada sme i da zameni virtual pozive direktnim.
// Ispravno: koristi Base kao član (kompozicija), ili ukloni final ako je
//   klasa zaista namenjena nasleđivanju.
class Base final {
public:
    virtual ~Base() = default;
};

class Derived : public Base {};

int main() {}
