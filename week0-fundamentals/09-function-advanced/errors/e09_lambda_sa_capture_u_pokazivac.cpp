// STD: c++17
// EXPECT-GCC: to 'int (*)()' in initialization
// EXPECT-CLANG: to 'int (*)()'
// POGREŠNO: samo lambda BEZ capture-a može da postane pokazivač na funkciju.
// Lambda sa capture-om je objekat sa stanjem, a pokazivač na funkciju nema
// gde da ga čuva.
// Ispravno: std::function<int()> f = [x]() { return x; };  ili  auto f = ...
int main() {
    int x = 1;
    int (*fp)() = [x]() { return x; };
    return fp();
}
