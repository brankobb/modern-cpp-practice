// EXPECT-GCC: variable or field 'nothing' declared void
// EXPECT-CLANG: variable has incomplete type 'void'
// POGREŠNO: void je tip "bez vrednosti" -- ne postoji objekat tipa void.
// void se koristi samo kao povratni tip ("ne vraća ništa") i kao void*
// (pokazivač na nepoznat tip, lekcija 04).
int main() {
    void nothing;
}
