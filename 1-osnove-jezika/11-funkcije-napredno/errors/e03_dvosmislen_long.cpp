// STD: c++17
// EXPECT-GCC: call of overloaded 'f(long int)' is ambiguous
// EXPECT-CLANG: call to 'f' is ambiguous
// POGREŠNO: long -> int i long -> double su OBE "konverzije" (isti rang),
// pa nijedan overload nije bolji -- poziv je dvosmislen.
// Ispravno: f(static_cast<int>(5L)), ili dodaj f(long).
void f(int) {}
void f(double) {}
int main() {
    f(5L);
}
