// STD: c++17
// EXPECT-GCC: as array of references
// EXPECT-CLANG: declared as array of references
// POGREŠNO: niz referenci ne postoji -- referenca nije objekat.
// Ispravno: niz pokazivača (int* arr[3]) ili std::array<std::reference_wrapper<int>, 3>
int main() {
    int a = 1, b = 2, c = 3;
    int& arr[3] = {a, b, c};
    return arr[0];
}
