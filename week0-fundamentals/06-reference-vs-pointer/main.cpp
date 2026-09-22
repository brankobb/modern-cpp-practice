#include <iostream>

struct Base {
    virtual void speak() const { std::cout << "Base\n"; }
    virtual ~Base() = default;
};
struct Derived : Base {
    void speak() const override { std::cout << "Derived\n"; }
};

void byValue(Base b) { b.speak(); }      // slicing -- uvek ispisuje "Base"
void byRef(const Base& b) { b.speak(); } // ispravno -- poziva pravu override verziju
void byPtr(const Base* b) { b->speak(); } // takođe ispravno, i može biti nullptr

int main() {
    Derived d;

    std::cout << "byValue: ";
    byValue(d); // slicing!

    std::cout << "byRef:   ";
    byRef(d);   // ispravno polimorfno ponašanje

    std::cout << "byPtr:   ";
    byPtr(&d);  // ispravno, i pokazuje da pointer dozvoljava "opciono" (nullptr)

    byPtr(nullptr); // TODO: ovo puca (nullptr dereference u speak()) -- pokreni pod ASan-om
}
