// EXPECT-UB: LeakSanitizer: detected memory leaks
// POGREŠNO: 2D niz od pokazivača na redove, a obrisan je samo niz pokazivača.
// Zašto: svaki red je posebna alokacija (new int[cols]). delete[] m oslobodi
//   samo niz POKAZIVAČA; redovi ostaju zauzeti i niko više nema njihove
//   adrese. Isto se desi ako izuzetak prekine petlju koja alocira redove.
// Ispravno: prvo delete[] m[r] za svaki red, pa delete[] m (main.cpp, 6a).
//   Bolje: jedan blok new int[rows * cols] ili std::vector, gde je jedno
//   oslobađanje dovoljno.
#include <cstdio>

int main() {
    int rows = 3;
    int cols = 4;
    int** m = new int*[rows];
    for (int r = 0; r < rows; ++r) m[r] = new int[cols]{};
    std::printf("%d\n", m[1][2]);
    delete[] m;
}
