// EXPECT-UB: heap-buffer-overflow
// POGREŠNO: petlja "i <= n" nad nizom od n elemenata na heap-u.
// Zašto: poslednji indeks je n - 1. new int[n] ne pamti granice na način koji
//   se proverava; upis u a[n] gazi tuđu memoriju (podatke alokatora ili drugi
//   objekat). Bez ASan-a program obično nastavi da radi i pukne kasnije,
//   negde drugde.
// Ispravno: i < n, ili std::vector<int> sa .at(i) / range-for.
#include <cstdio>

int main() {
    int n = 5;
    int* a = new int[n];
    for (int i = 0; i <= n; ++i) a[i] = i;
    std::printf("%d\n", a[0]);
    delete[] a;
}
