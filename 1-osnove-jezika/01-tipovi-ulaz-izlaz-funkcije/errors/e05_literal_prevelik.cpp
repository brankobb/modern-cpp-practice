// EXPECT-GCC: integer constant is too large for its type
// EXPECT-CLANG: integer literal is too large to be represented in any integer type
// POGREŠNO: celobrojni literal mora da stane u neki celobrojni tip
// (najveći je unsigned long long, 64 bita: 18446744073709551615).
// Ispravno: manji broj, ili double literal (1.2345678901234568e+23) ako
// ti ne treba tačnost do jedinice.
int main() {
    unsigned long long x = 123456789012345678901234;
    return static_cast<int>(x);
}
