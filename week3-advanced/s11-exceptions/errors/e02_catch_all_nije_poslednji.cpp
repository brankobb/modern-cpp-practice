// EXPECT-GCC: '...' handler must be the last handler for its try block
// EXPECT-CLANG: catch-all handler must come last
// POGREŠNO: catch (...) hvata SVE, pa posle njega ništa ne bi moglo da se
// izvrši. Ovde je standard strožiji nego u e01: to je greška, ne upozorenje.
// Ispravno: catch (...) uvek poslednji.
int main() {
    try {
        throw 1;
    } catch (...) {
    } catch (int) {
    }
}
