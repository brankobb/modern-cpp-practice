// STD: c++17
// EXPECT-GCC: lvalue required as unary '&' operand
// EXPECT-CLANG: cannot take the address of an rvalue
// POGREŠNO: & (adresa) traži lvalue -- literal 5 nema adresu.
// Ispravno: int x = 5; int* p = &x;
int main() {
    int* p = &5;
    return *p;
}
