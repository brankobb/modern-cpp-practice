// EXPECT-UB: null pointer|SEGV|stack-use-after-return
// UB (EMC Item 3): decltype(auto) sa "return (x);" vraća int& -- (x) je
// izraz, a decltype izraza-lvalue je referenca. Ovde je to referenca na
// lokalnu promenljivu. Jedan par zagrada menja povratni tip.
// Ispravno: return x;  (bez zagrada -> int)
#include <iostream>
decltype(auto) value() {
    int x = 42;
    return (x);
}
int main() {
    std::cout << value() << "\n";
}
