// STD: c++17
// EXPECT-GCC: narrowing conversion
// EXPECT-CLANG: cannot be narrowed
// POGREŠNO: narrowing se određuje po TIPU, ne po vrednosti -- ll je 1, ali
// long long ne staje uvek u int, pa je ovo narrowing.
// Pažnja: sa "long" umesto "long long" rezultat zavisi od platforme -- na
// Windows-u je long 32-bitni (isto kao int) pa to NIJE narrowing, na Linux-u
// jeste (long je 64-bitni).
int main() {
    long long ll = 1;
    int i{ll};
    return i;
}
