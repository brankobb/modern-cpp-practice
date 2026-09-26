// STD: c++17
// EXPECT-GCC: too many initializers for 'int [3]'
// EXPECT-CLANG: excess elements in array initializer
// POGREŠNO: new int[3] sa četiri vrednosti u {}.
// Zašto: kad je dužina konstanta, kompajler proverava broj inicijalizatora
//   kao za običan niz (lekcija 05). Kad je dužina poznata tek u toku
//   izvršavanja, ista greška postaje izuzetak: standard traži
//   std::bad_array_new_length (g++), a clang baca std::bad_alloc.
// Ispravno: new int[4]{1, 2, 3, 4}, ili new int[]{1, 2, 3, 4} (veličina iz liste).
int main() {
    int* p = new int[3]{1, 2, 3, 4};
    delete[] p;
}
