// EXPECT-UB: member access within null pointer of type 'const (struct )?Sensor'
// BAG za vežbu (notes.md, sekcija 4): find() vraća nullptr za nepoznat ID,
// a pozivalac ne proverava. UBSan prijavi pristup članu kroz null
// pokazivač (i nastavi -- pa zatim ASan prijavi SEGV). Bez sanitizera:
// "Segmentation fault", a u gdb-u "bt" pokaže readId (s=0x0) <- main.
// Ispravno: proveri rezultat (if (const Sensor* s = find(2)) ...), ili
// neka find() vrati nešto što ne može da bude "prazno" bez provere
// (npr. std::optional<Sensor>).
#include <iostream>

struct Sensor {
    int id;
};

Sensor global{1};

const Sensor* find(int id) { return id == 1 ? &global : nullptr; }
int readId(const Sensor* s) { return s->id; }

int main(int argc, char**) {
    std::cout << readId(find(argc + 1)) << '\n';
}
