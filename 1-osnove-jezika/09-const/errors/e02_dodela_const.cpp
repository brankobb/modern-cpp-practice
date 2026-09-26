// STD: c++17
// EXPECT-GCC: assignment of read-only variable 'x'
// EXPECT-CLANG: with const-qualified type 'const int'
// POGREŠNO: const objekat se posle inicijalizacije ne menja.
int main() {
    const int x = 1;
    x = 2;
    return x;
}
