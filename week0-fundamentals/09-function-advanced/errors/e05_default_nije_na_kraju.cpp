// STD: c++17
// EXPECT-GCC: default argument missing for parameter 2
// EXPECT-CLANG: missing default argument on parameter 'b'
// POGREŠNO: posle parametra sa podrazumevanom vrednošću svi naredni moraju
// da je imaju -- argumenti se popunjavaju s leva, pa b ne bi imao vrednost.
// Ispravno: void f(int b, int a = 1);
void f(int a = 1, int b) {}
int main() {}
