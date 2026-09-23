// EXPECT-GCC: specialization of 'void obradi(T) [with T = int]' after instantiation
// EXPECT-CLANG: explicit specialization of 'obradi<int>' after instantiation
// POGREŠNO: obradi(1) je već instancirao opšti šablon za int. Specijalizacija
// koja dođe posle toga bi značila da isti poziv u različitim delovima
// programa radi različite stvari.
// Ispravno: specijalizacija mora biti deklarisana PRE prve upotrebe -- u
// praksi odmah uz šablon, u istom header-u.
template <typename T>
void obradi(T) {}
int koristi() {
    obradi(1);
    return 0;
}
template <>
void obradi<int>(int) {}
int main() { return koristi(); }
