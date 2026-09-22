// STD: c++17
// EXPECT-GCC: cannot convert 'int*' to 'double*'
// EXPECT-CLANG: of type 'double *' with an rvalue of type 'int *'
// POGREŠNO: nema implicitne konverzije između pokazivača na različite tipove.
// double* bi čitao 8 bajtova sa adrese gde je int od 4 bajta.
int main() {
    int x = 1;
    double* pd = &x;
    return static_cast<int>(*pd);
}
