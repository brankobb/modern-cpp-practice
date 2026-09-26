// STD: c++17
// EXPECT-GCC: narrowing conversion
// EXPECT-CLANG: cannot be narrowed
// POGREŠNO: double promenljiva u float unutar {}.
// Zanimljivo: float f{0.1}; JE dozvoljeno -- konstanta u opsegu float-a nije
// narrowing čak i kad nije tačno predstavljiva. Promenljiva jeste.
int main() {
    double d = 0.1;
    float f{d};
    return static_cast<int>(f);
}
