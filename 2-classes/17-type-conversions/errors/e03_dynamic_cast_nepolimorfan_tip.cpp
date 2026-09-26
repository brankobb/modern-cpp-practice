// STD: c++17
// EXPECT-GCC: (source type is not polymorphic)
// EXPECT-CLANG: 'Animal' is not polymorphic
// POGREŠNO: dynamic_cast naniže kroz klasu bez virtual funkcija.
// Zašto: dynamic_cast pri izvršavanju čita informaciju o tipu preko vptr-a
//   (lekcija 16, sekcija 7). Klasa bez ijedne virtual funkcije nema vptr,
//   pa nema ni odakle da se pročita stvarni tip.
// Ispravno: virtual ~Animal() = default; u baznoj klasi (i tako treba za
//   svaku polimorfnu bazu, EC++ Item 7).
struct Animal {
    int legs = 4;
};
struct Dog : Animal {
    int volume = 5;
};

int main() {
    Dog d;
    Animal* a = &d;
    Dog* p = dynamic_cast<Dog*>(a);
    return p->volume;
}
