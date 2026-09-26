// EXPECT-GCC: return-statement with no value, in function returning 'int'
// EXPECT-CLANG: non-void function 'procitaj' should return a value
// POGREŠNO: funkcija koja vraća int mora u svakom return-u da vrati vrednost.
// (Izlazak sa KRAJA funkcije bez return-a se kompajlira, ali je UB: ub/u06.)
// Ispravno: return 0; ili vrati kod greške / std::optional<int>.
int procitaj(bool ok) {
    if (!ok) return;
    return 42;
}
int main() { return procitaj(true) == 42 ? 0 : 1; }
