// STD: c++17
// EXPECT-GCC: assignment of read-only variable 'p'
// EXPECT-CLANG: const-qualified type 'int *const'
// POGREŠNO: int* const -- sam pokazivač je const, ne može da se preusmeri.
// (Vrednost sme da se menja: *p = 5; je OK.)
int main() {
    int x = 1;
    int y = 2;
    int* const p = &x;
    p = &y;
    return *p;
}
