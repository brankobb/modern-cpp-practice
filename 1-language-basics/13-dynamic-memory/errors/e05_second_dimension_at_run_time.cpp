// STD: c++17
// EXPECT-GCC: array size in new-expression must be constant
// EXPECT-CLANG: array size is not a constant expression
// POGREŠNO: new int[rows][cols] kada je cols poznat tek u toku izvršavanja.
// Zašto: samo PRVA dimenzija u new sme da bude runtime vrednost. Ostale su
//   deo tipa elementa (int[cols]), a tip mora biti poznat pri kompajliranju.
//   g++ bez -pedantic-errors ovo odbija isto, samo bez poruke o VLA.
// Ispravno: jedan blok new int[rows * cols] sa indeksom r * cols + c, ili
//   std::vector<int>(rows * cols) (main.cpp, sekcija 6).
int main(int argc, char**) {
    int rows = 2;
    int cols = argc + 3;
    auto m = new int[rows][cols];
    delete[] m;
}
