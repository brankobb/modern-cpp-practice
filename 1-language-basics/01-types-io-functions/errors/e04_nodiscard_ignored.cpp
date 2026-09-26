// FLAGS: -Werror=unused-result
// EXPECT-GCC: ignoring return value of 'bool send(int)', declared with attribute 'nodiscard'
// EXPECT-CLANG: ignoring return value of function declared with 'nodiscard' attribute
// POGREŠNO: rezultat [[nodiscard]] funkcije je jedini signal greške, a
// ignorisan je. Bez -Werror je samo upozorenje (-Wunused-result); ovde je
// greška, kao u build-u koji upozorenja tretira kao greške.
// Ispravno: if (!send(7)) { ...obradi grešku... }
// (Ako baš hoćeš da ga ignorišeš, napiši to: (void)send(7);)
[[nodiscard]] bool send(int byte) { return byte >= 0; }
int main() { send(7); }
