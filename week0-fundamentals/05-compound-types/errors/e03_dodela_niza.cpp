// STD: c++17
// EXPECT-GCC: invalid array assignment
// EXPECT-CLANG: array type 'int[3]' is not assignable
// POGREŠNO: C nizovi se ne mogu dodeljivati (ni kopirati ni porediti kao celina).
// Ispravno: std::array<int, 3> a{}, b{1, 2, 3}; a = b;  -- ili std::copy
int main() {
    int a[3]{};
    int b[3]{1, 2, 3};
    a = b;
    return a[0];
}
