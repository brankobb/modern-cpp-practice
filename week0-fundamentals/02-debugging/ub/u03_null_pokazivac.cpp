// EXPECT-UB: member access within null pointer of type 'const (struct )?Senzor'
// BAG za vežbu (notes.md, sekcija 4): nadji() vraća nullptr za nepoznat ID,
// a pozivalac ne proverava. UBSan prijavi pristup članu kroz null
// pokazivač (i nastavi -- pa zatim ASan prijavi SEGV). Bez sanitizera:
// "Segmentation fault", a u gdb-u "bt" pokaže procitaj (s=0x0) <- main.
// Ispravno: proveri rezultat (if (const Senzor* s = nadji(2)) ...), ili
// neka nadji() vrati nešto što ne može da bude "prazno" bez provere
// (npr. std::optional<Senzor>).
#include <iostream>

struct Senzor {
    int id;
};

Senzor globalni{1};

const Senzor* nadji(int id) { return id == 1 ? &globalni : nullptr; }
int procitaj(const Senzor* s) { return s->id; }

int main(int argc, char**) {
    std::cout << procitaj(nadji(argc + 1)) << '\n';
}
