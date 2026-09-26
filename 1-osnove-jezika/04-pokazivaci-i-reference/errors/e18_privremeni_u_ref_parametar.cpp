// STD: c++17
// EXPECT-GCC: cannot bind non-const lvalue reference of type 'int&' to an rvalue
// EXPECT-CLANG: no matching function for call to 'inc'
// POGREŠNO: funkcija koja prima T& ne prima privremenu vrednost --
// izmena bi otišla u objekat koji odmah nestaje, pa jezik to zabranjuje.
// Ispravno: void inc(int& v) se poziva sa promenljivom: int n = 5; inc(n);
void inc(int& v) {
    ++v;
}
int main() {
    inc(5);
}
