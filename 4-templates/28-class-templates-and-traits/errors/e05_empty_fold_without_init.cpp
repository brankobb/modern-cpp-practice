// EXPECT-GCC: fold of empty expansion over operator+
// EXPECT-CLANG: unary fold expression has empty expansion for operator '+' with no fallback value
// POGREŠNO: unarni fold (args + ...) za PRAZAN pack nema vrednost -- šta
// je "zbir ničega"? Samo za &&, || i zarez standard propisuje vrednost
// praznog folda (true, false, void()).
// Ispravno: binarni fold sa početnom vrednošću, (0 + ... + args) -- za
// prazan pack daje 0 (main.cpp, sekcija 2).
template <typename... Args>
auto sum(Args... args) {
    return (args + ...);
}
int main() { return sum(); }
