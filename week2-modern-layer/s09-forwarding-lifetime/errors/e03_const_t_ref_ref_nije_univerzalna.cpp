// STD: c++17
// EXPECT-GCC: cannot bind rvalue reference of type 'const int&&' to lvalue of type 'int'
// EXPECT-CLANG: no matching function for call to 'observe'
// POGREŠNO: const T&& očekivan kao forwarding referenca.
// Zašto: i jedan kvalifikator (const) uz T&& ga pretvara u običnu rvalue
//   referencu (EMC Item 24). Samo golo "T&&" (i "auto&&") je forwarding.
// Ispravno: template <typename T> void observe(const T& x) ako samo čita,
//   ili T&& ako treba i da prosledi dalje.
template <typename T>
void observe(const T&& x) { (void)x; }

int main() {
    int a = 1;
    observe(a);
}
