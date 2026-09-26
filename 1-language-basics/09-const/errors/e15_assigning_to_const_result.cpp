// STD: c++17
// EXPECT-GCC: as 'this' argument discards qualifiers
// EXPECT-CLANG: no viable overloaded '='
// POGREŠNO (EC++ Item 3): kad operator* vraća const objekat, besmislena
// dodela rezultatu -- (a * b) = c; -- postaje greška pri kompajliranju.
// Moderna alternativa bez gubitka move-a: ref-qualifier na operator=
// (Rational& operator=(const Rational&) &;) -- vidi main.cpp, sekcija 6.
struct Rational {
    int n = 0;
};
const Rational operator*(const Rational& a, const Rational& b) {
    return Rational{a.n * b.n};
}
int main() {
    Rational a{2}, b{3}, c{4};
    (a * b) = c;
}
