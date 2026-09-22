// STD: c++17
// EXPECT-GCC: invalid conversion from 'const int*' to 'int*'
// EXPECT-CLANG: of type 'int *' with an rvalue of type 'const int *'
// POGREŠNO: int* na const objekat bi dozvolio izmenu const objekta.
// Dodavanje const je implicitno (int* -> const int*), skidanje nije.
// Ispravno: const int* p = &c;
int main() {
    const int c = 1;
    int* p = &c;
    return *p;
}
