// FLAGS: -Werror=unused-result
// EXPECT-GCC: ignoring return value of 'bool posalji(int)', declared with attribute 'nodiscard'
// EXPECT-CLANG: ignoring return value of function declared with 'nodiscard' attribute
// POGREŠNO: rezultat [[nodiscard]] funkcije je jedini signal greške, a
// ignorisan je. Bez -Werror je samo upozorenje (-Wunused-result); ovde je
// greška, kao u build-u koji upozorenja tretira kao greške.
// Ispravno: if (!posalji(7)) { ...obradi grešku... }
// (Ako baš hoćeš da ga ignorišeš, napiši to: (void)posalji(7);)
[[nodiscard]] bool posalji(int bajt) { return bajt >= 0; }
int main() { posalji(7); }
