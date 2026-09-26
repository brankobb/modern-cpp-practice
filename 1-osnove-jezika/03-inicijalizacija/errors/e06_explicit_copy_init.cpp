// STD: c++17
// EXPECT-GCC: to non-scalar type 'ExplicitOnly'
// EXPECT-CLANG: no viable conversion from 'int'
// POGREŠNO: explicit konstruktor kroz copy-initialization (=).
// Zašto: kod copy-init explicit konstruktori se uopšte ne razmatraju.
// Ispravno: ExplicitOnly e(5);  ili  ExplicitOnly e{5};
struct ExplicitOnly {
    explicit ExplicitOnly(int) {}
};
int main() {
    ExplicitOnly e = 5;
    (void)e;
}
