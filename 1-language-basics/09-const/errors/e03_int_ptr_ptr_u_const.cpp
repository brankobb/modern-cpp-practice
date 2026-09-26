// STD: c++17
// EXPECT-GCC: invalid conversion from 'int**' to 'const int**'
// EXPECT-CLANG: of type 'const int **' with an lvalue of type 'int **'
// POGREŠNO (i iznenađujuće): int** -> const int** NIJE dozvoljeno, iako
// int* -> const int* jeste. Da jeste, ovako bi se tiho menjao const objekat:
//   const int c = 1;  int* p;  const int** cpp = &p;  *cpp = &c;  *p = 2;
// Ispravno: const int* const* cpp = pp;  -- tu se ne može upisati u *cpp
int main() {
    int x = 1;
    int* p = &x;
    int** pp = &p;
    const int** cpp = pp;
    return **cpp;
}
