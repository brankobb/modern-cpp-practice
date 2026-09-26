// EXPECT-GCC: 'kvadrat' was not declared in this scope
// EXPECT-CLANG: use of undeclared identifier 'kvadrat'
// POGREŠNO: ime mora biti deklarisano PRE upotrebe; kompajler čita odozgo nadole.
// Ispravno: deklaracija (prototip) iznad main-a:  int kvadrat(int x);
// a definicija može ispod, ili u drugom .cpp fajlu (lekcija 08).
int main() { return kvadrat(3) == 9 ? 0 : 1; }
int kvadrat(int x) { return x * x; }
