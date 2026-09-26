// STD: c++17
// EXPECT-GCC: cannot bind non-const lvalue reference of type 'int&' to a value of type 'double'
// EXPECT-CLANG: cannot bind to a value of unrelated type 'double'
// POGREŠNO: int& ne može da se veže za double -- trebao bi privremeni int.
// Iznenađenje: const int& r = d; JE dozvoljeno, ali se veže za privremenu
// KOPIJU (1), ne za d -- kasnija izmena d se kroz r ne vidi (main.cpp, sekcija 8).
int main() {
    double d = 1.5;
    int& r = d;
    return r;
}
