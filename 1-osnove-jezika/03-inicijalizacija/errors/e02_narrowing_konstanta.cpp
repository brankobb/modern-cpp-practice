// STD: c++17
// EXPECT-GCC: narrowing conversion
// EXPECT-CLANG: cannot be narrowed
// POGREŠNO: narrowing iz konstante koja ne staje tačno u int.
// Ispravno: int x{3};  (konstanta koja tačno staje nije narrowing)
int main() {
    int x{3.14};
    return x;
}
