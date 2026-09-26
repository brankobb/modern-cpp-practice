// STD: c++17
// EXPECT-GCC: cannot bind non-const lvalue reference of type 'int&' to an rvalue
// EXPECT-CLANG: cannot bind to a temporary
// POGREŠNO: obična (non-const) lvalue referenca ne može da se veže za
// privremenu vrednost (rvalue).
// Ispravno: const int& r = 5;  (i životni vek privremenog se produžava)
int main() {
    int& r = 5;
    return r;
}
