// STD: c++17
// EXPECT-GCC: as array of functions
// EXPECT-CLANG: declared as array of functions
// POGREŠNO: niz funkcija ne postoji (funkcija nije objekat).
// Ispravno: niz POKAZIVAČA na funkcije -- void (*handlers[2])(int);
// ili čitljivije: using Handler = void (*)(int); Handler handlers[2];
void (handlers[2])(int);
int main() {}
