// STD: c++17
// EXPECT-GCC: inconsistent deduction for auto return type: 'int' and then 'double'
// EXPECT-CLANG: deduced as 'int' in earlier return statement
// POGREŠNO: auto povratni tip mora biti ISTI u svim return naredbama.
// Ispravno: double pick(bool c) { ... }  ili  return 1.0;
auto pick(bool c) {
    if (c) return 1;
    return 2.0;
}
int main() {
    return static_cast<int>(pick(true));
}
