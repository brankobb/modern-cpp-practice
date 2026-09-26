// FLAGS: -Werror=return-type
// EXPECT-GCC: control reaches end of non-void function
// EXPECT-CLANG: non-void function does not return a value in all control paths
// ZAŠTO -Werror: bez njega je ovo samo upozorenje, program se kompajlira,
// a izlazak sa kraja funkcije bez return-a je UB (lekcija 01, ub/u06).
// Upozorenje se lako previdi među ostalima; kao greška ne može.
// Ispravno: return na svakoj putanji. U svom projektu razmisli o -Werror
// (bar za -Wreturn-type), i drži build bez ijednog upozorenja.
int grade(int points) {
    if (points >= 90) return 10;
    if (points >= 50) return 6;
}
int main() { return grade(95) == 10 ? 0 : 1; }
