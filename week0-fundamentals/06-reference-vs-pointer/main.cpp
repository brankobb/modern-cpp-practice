#include <iostream>

struct Base {
    virtual void speak() const { std::cout << "Base\n"; }
    virtual ~Base() = default;
};
struct Derived : Base {
    void speak() const override { std::cout << "Derived\n"; }
};

// Ako prosleđuješ polimorfni tip PO VREDNOSTI (NIJE DOBRO) jer se dešava
// slicing -- kopira se SAMO Base deo objekta, Derived deo se odseca, pa
// pozivi virtualnih funkcija uvek idu na Base verziju bez obzira na
// stvarni tip argumenta.
void byValue(Base b) { b.speak(); }      // slicing -- uvek ispisuje "Base"

// Treba da prosleđuješ REFERENCU (ili pokazivač) kad ti je bitno
// polimorfno ponašanje -- referenca/pokazivač ne kopira objekat, samo
// "gleda" na njega, pa se virtual dispatch odvija na STVARNOM tipu.
void byRef(const Base& b) { b.speak(); } // ispravno -- poziva pravu override verziju

// Možeš i pokazivačem kad ti dodatno treba "opciono" ponašanje (objekat
// možda ne postoji -- nullptr) ili menjaš NA ŠTA pokazuje tokom vremena.
void byPtr(const Base* b) { b->speak(); } // takođe ispravno, i može biti nullptr

int main() {
    // unitbuf -- auto-flush posle svake cout operacije, da ispis ne
    // ostane zaglavljen u baferu ako program pukne pre nego što se
    // isprazni (bitno kad je stdout preusmeren u fajl, ne terminal).
    std::cout.setf(std::ios::unitbuf);

    Derived d;

    std::cout << "byValue: ";
    byValue(d); // slicing!

    std::cout << "byRef:   ";
    byRef(d);   // ispravno polimorfno ponašanje

    std::cout << "byPtr:   ";
    byPtr(&d);  // ispravno, i pokazuje da pointer dozvoljava "opciono" (nullptr)

    std::cout << "byPtr(nullptr) -- namerni crash, ASan treba da uhvati:\n";
    byPtr(nullptr); // TODO: ovo puca (nullptr dereference u speak()) -- pokreni pod ASan-om
}
