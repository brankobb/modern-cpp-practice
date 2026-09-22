// STD: c++17
// EXPECT-GCC: call of overloaded 'f(int)' is ambiguous
// EXPECT-CLANG: call to 'f' is ambiguous
// POGREŠNO: int -> long i int -> double su obe konverzije (nijedna nije
// promocija), pa je f(5) dvosmislen -- iako "5 izgleda kao long".
void f(long) {}
void f(double) {}
int main() {
    f(5);
}
