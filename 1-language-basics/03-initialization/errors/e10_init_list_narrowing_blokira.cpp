// STD: c++17
// EXPECT-GCC: from 'int' to 'bool'
// EXPECT-CLANG: cannot be narrowed to type 'bool'
// POGREŠNO (EMC Item 7): initializer_list konstruktor "blokira" savršen match.
// W(int, double) bi tačno odgovarao za {10, 5.0}, ali kompajler PRVO bira
// initializer_list<bool> ctor, a konverzija 10 -> bool i 5.0 -> bool je
// narrowing -> greška. Kompajler se NE vraća na W(int, double).
// Ispravno: W w(10, 5.0);  -- zagrade ne gledaju initializer_list ctor
#include <initializer_list>
struct W {
    W(int, bool) {}
    W(int, double) {}
    W(std::initializer_list<bool>) {}
};
int main() {
    W w{10, 5.0};
    (void)w;
}
