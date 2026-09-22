// STD: c++17
// EXPECT-GCC: cannot bind non-const lvalue reference of type 'int&' to an rvalue
// EXPECT-CLANG: cannot bind to a temporary of type 'int'
// POGREŠNO: auto& je obična lvalue referenca -- ne veže se za privremenu vrednost.
// Ispravno: const auto& r = 42;  ili  auto&& r = 42;  (vidi main.cpp, sekcija 3)
int main() {
    auto& r = 42;
    return r;
}
