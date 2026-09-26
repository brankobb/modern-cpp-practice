// STD: c++17
// EXPECT-GCC: narrowing conversion
// EXPECT-CLANG: cannot be narrowed
// POGREŠNO: konstanta van opsega ciljnog tipa.
// Ispravno: unsigned u{1};  ili  int i{-1};
int main() {
    unsigned u{-1};
    return static_cast<int>(u);
}
