// EXPECT-UB: heap-buffer-overflow
// POGREŠNO: 2D niz u jednom bloku sa zamenjenim redom i kolonom u indeksu.
// Zašto: element (r, c) je na r * cols + c. Ovde piše c * cols + r; za
//   rows != cols najveći indeks je (cols - 1) * cols + (rows - 1) = 21, a
//   blok ima rows * cols = 10 elemenata. Kod kvadratne matrice ista greška
//   ne izlazi iz bloka, nego tiho transponuje podatke; nijedan alat to ne hvata.
// Ispravno: m[r * cols + c], najbolje sakriveno u jednoj funkciji
//   (int& at(int r, int c)) da se formula piše na jednom mestu.
#include <cstdio>

int main() {
    int rows = 2;
    int cols = 5;
    int* m = new int[rows * cols]{};
    for (int r = 0; r < rows; ++r)
        for (int c = 0; c < cols; ++c)
            m[c * cols + r] = 1;
    std::printf("%d\n", m[0]);
    delete[] m;
}
