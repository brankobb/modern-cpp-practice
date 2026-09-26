// STD: c++17
// EXPECT-GCC: assignment of read-only reference 'r'
// EXPECT-CLANG: with const-qualified type 'const int &'
// POGREŠNO: kroz const referencu se ne piše (iako sam x nije const).
int main() {
    int x = 1;
    const int& r = x;
    r = 5;
    return x;
}
