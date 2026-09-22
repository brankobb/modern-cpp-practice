// STD: c++17
// EXPECT-GCC: default argument given for parameter 1
// EXPECT-CLANG: redefinition of default argument
// POGREŠNO: podrazumevana vrednost se navodi JEDNOM -- obično u deklaraciji
// (u header-u), ne i u definiciji.
// Ispravno: void f(int x = 1);   ...   void f(int x) { }
void f(int x = 1);
void f(int x = 1) {}
int main() {
    f();
}
