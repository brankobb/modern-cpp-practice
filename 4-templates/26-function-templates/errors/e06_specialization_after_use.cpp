// EXPECT-GCC: specialization of 'void process(T) [with T = int]' after instantiation
// EXPECT-CLANG: explicit specialization of 'process<int>' after instantiation
// POGREŠNO: process(1) je već instancirao opšti šablon za int. Specijalizacija
// koja dođe posle toga bi značila da isti poziv u različitim delovima
// programa radi različite stvari.
// Ispravno: specijalizacija mora biti deklarisana PRE prve upotrebe -- u
// praksi odmah uz šablon, u istom header-u.
template <typename T>
void process(T) {}
int caller() {
    process(1);
    return 0;
}
template <>
void process<int>(int) {}
int main() { return caller(); }
