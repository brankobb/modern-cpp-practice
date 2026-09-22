// EXPECT-UB: load of null pointer|SEGV
// UB: dereferenciranje null pokazivača. Kompajlira se bez upozorenja.
// Ispravno: proveri pre upotrebe -- if (p) { ... } -- ili prosledi referencu
// ako vrednost MORA da postoji.
#include <iostream>
int main() {
    int* p = nullptr;
    std::cout << *p << "\n";
}
