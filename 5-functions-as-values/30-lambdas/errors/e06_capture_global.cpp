// EXPECT-GCC: capture of variable 'counter' with non-automatic storage duration
// EXPECT-CLANG: 'counter' cannot be captured because it does not have automatic storage duration
// POGREŠNO: zarobljavaju se samo LOKALNE (automatske) promenljive. Globalne
// i static postoje ceo program, pa ih lambda koristi direktno, bez
// capture-a -- i uvek vidi TRENUTNU vrednost, ne kopiju. (g++ bez
// -pedantic-errors samo upozori.)
// Ispravno: [] { return brojac; } -- ili, ako ti treba snimak vrednosti,
// init capture [copy = counter] { return copy; }.
int counter = 5;
int main() {
    auto f = [counter] { return counter; };
    return f();
}
