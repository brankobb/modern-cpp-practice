// EXPECT-UB: member access within null pointer of type 'struct Dog'
// POGREŠNO: rezultat dynamic_cast<Dog*> se koristi bez provere.
// Zašto: kad objekat nije Dog, dynamic_cast vrati nullptr. To je cela
//   poenta: pitanje "da li je ovo Dog?" ima odgovor "ne". Ako se odgovor ne
//   pogleda, d->volume je pristup preko null pokazivača.
// Ispravno: if (Dog* d = dynamic_cast<Dog*>(&a)) { ... } (main.cpp,
//   sekcija 4), ili dynamic_cast<Dog&> koji baca std::bad_cast.
#include <cstdio>

struct Animal {
    virtual ~Animal() = default;
};
struct Dog : Animal {
    int volume = 5;
};
struct Cat : Animal {
    int lives = 9;
};

int loudness(Animal& a) {
    Dog* d = dynamic_cast<Dog*>(&a);
    return d->volume;
}

int main() {
    Cat c;
    std::printf("%d\n", loudness(c));
}
