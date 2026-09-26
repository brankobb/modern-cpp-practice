// EXPECT-GCC: 'square' was not declared in this scope
// EXPECT-CLANG: use of undeclared identifier 'square'
// POGREŠNO: ime mora biti deklarisano PRE upotrebe; kompajler čita odozgo nadole.
// Ispravno: deklaracija (prototip) iznad main-a:  int square(int x);
// a definicija može ispod, ili u drugom .cpp fajlu (lekcija 08).
int main() { return square(3) == 9 ? 0 : 1; }
int square(int x) { return x * x; }
