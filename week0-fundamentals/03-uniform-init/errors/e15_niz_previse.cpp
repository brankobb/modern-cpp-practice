// STD: c++17
// EXPECT-GCC: too many initializers
// EXPECT-CLANG: excess elements in array initializer
// POGREŠNO: više inicijalizatora nego elemenata niza.
// Ispravno: int arr[]{1, 2, 3};  (veličina se dedukuje)
int main() {
    int arr[2]{1, 2, 3};
    return arr[0];
}
