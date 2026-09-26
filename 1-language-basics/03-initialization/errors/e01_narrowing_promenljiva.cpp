// STD: c++17
// EXPECT-GCC: narrowing conversion
// EXPECT-CLANG: cannot be narrowed
// POGREŠNO: narrowing iz promenljive unutar {}.
// Zašto: {} zabranjuje gubitak podataka (double -> int).
// Bez -pedantic-errors g++ ovo prijavljuje samo kao WARNING i kompajlira (standard
// traži samo "dijagnostiku"), clang uvek daje grešku. Build zato koristi -pedantic-errors.
// Ispravno: int x = static_cast<int>(d);  (namerna konverzija, vidljiva u kodu)
int main() {
    double d = 3.14;
    int x{d};
    return x;
}
