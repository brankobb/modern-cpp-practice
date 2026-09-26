// STD: c++17
// EXPECT-GCC: expected identifier before numeric constant
// EXPECT-CLANG: expected parameter declarator
// POGREŠNO: default vrednost člana klase sa ().
// Zašto: gramatika dozvoljava samo {} ili = za default member initializer
// (EMC Item 7); kompajler pokušava da pročita liniju kao deklaraciju funkcije.
// Ispravno: int c{3};  ili  int c = 3;
struct S {
    int c(3);
};
int main() {}
