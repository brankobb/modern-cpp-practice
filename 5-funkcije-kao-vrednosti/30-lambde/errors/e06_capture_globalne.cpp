// EXPECT-GCC: capture of variable 'brojac' with non-automatic storage duration
// EXPECT-CLANG: 'brojac' cannot be captured because it does not have automatic storage duration
// POGREŠNO: zarobljavaju se samo LOKALNE (automatske) promenljive. Globalne
// i static postoje ceo program, pa ih lambda koristi direktno, bez
// capture-a -- i uvek vidi TRENUTNU vrednost, ne kopiju. (g++ bez
// -pedantic-errors samo upozori.)
// Ispravno: [] { return brojac; } -- ili, ako ti treba snimak vrednosti,
// init capture [kopija = brojac] { return kopija; }.
int brojac = 5;
int main() {
    auto f = [brojac] { return brojac; };
    return f();
}
