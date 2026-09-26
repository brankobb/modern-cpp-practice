// EXPECT-UB: null pointer|SEGV|stack-use-after-return
// UB (EC++ Item 21): isto kao u03, samo preko reference -- sintaksa je
// "tiša" (nema & ni *), a greška ista.
#include <iostream>
int& make() {
    int local = 42;
    return local;
}
int main() {
    int& r = make();
    std::cout << r << "\n";
}
