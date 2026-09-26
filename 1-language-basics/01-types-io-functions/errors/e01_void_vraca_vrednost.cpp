// EXPECT-GCC: return-statement with a value, in function returning 'void'
// EXPECT-CLANG: void function 'loguj' should not return a value
// POGREŠNO: void funkcija nema povratnu vrednost ([stmt.return]).
// Ispravno: return; (ili ništa), ili promeni povratni tip u int ako
// pozivalac treba rezultat.
void loguj(int x) {
    if (x < 0) return -1;
}
int main() { loguj(1); }
