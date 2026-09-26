// STD: c++17
// LINK: support/empty.cpp
// EXPECT-GCC: undefined reference to `Base::~Base()'
// EXPECT-CLANG: undefined reference to `Base::~Base()'
// POGREŠNO: "virtual ~Base() = 0;" bez definicije.
// Zašto: pure virtual destruktor čini klasu apstraktnom, ali se i dalje
//   POZIVA: destruktor izvedene klase na kraju uvek pozove destruktor baze
//   (lekcija 19, redosled destrukcije). "= 0" znači "izvedena klasa mora da
//   nadjača", a ne "ne postoji". Kompajler to ne proverava; javi linker.
// Ispravno: Base::~Base() = default; (ili {}) van klase (main.cpp,
//   sekcija 12). Isto važi za svaku pure virtual funkciju koja se poziva
//   kvalifikovano (Base::f()).
struct Base {
    virtual ~Base() = 0;
};

struct Derived : Base {};

int main() {
    Derived d;
    (void)d;
}
