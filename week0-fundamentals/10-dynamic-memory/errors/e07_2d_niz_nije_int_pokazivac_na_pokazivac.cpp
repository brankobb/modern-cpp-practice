// STD: c++17
// EXPECT-GCC: cannot convert 'int (*)[4]' to 'int**' in initialization
// EXPECT-CLANG: cannot initialize a variable of type 'int **' with an rvalue of type 'int (*)[4]'
// POGREŠNO: rezultat new int[3][4] dodeljen int**.
// Zašto: new int[3][4] pravi JEDAN blok od 12 int-ova i vraća pokazivač na
//   prvi red, tipa int (*)[4]. int** je pokazivač na POKAZIVAČ, a u tom
//   bloku nema nijednog pokazivača. Isto kao u lekciji 05: int[3][4] se
//   raspada u int (*)[4], ne u int**.
// Ispravno: int (*m)[4] = new int[3][4]; ili auto m = new int[3][4];
//   int** je za niz pokazivača na posebno alocirane redove (main.cpp, sekcija 6a).
int main() {
    int** m = new int[3][4];
    delete[] m;
}
