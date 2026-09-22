// STD: c++17
// EXPECT-GCC: cannot bind rvalue reference of type 'int&&' to lvalue
// EXPECT-CLANG: rvalue reference to type 'int' cannot bind to lvalue
// POGREŠNO: rvalue referenca (int&&) se ne vezuje za lvalue.
// Ispravno: int&& r = std::move(x);  ili  int&& r = 5;
int main() {
    int x = 1;
    int&& r = x;
    return r;
}
