// STD: c++17
// EXPECT-GCC: constructors cannot be declared 'virtual'
// EXPECT-CLANG: constructor cannot be declared 'virtual'
// POGREŠNO: virtual konstruktor.
// Zašto: virtual poziv bira funkciju po stvarnom tipu objekta, a pri
//   konstrukciji objekat još ne postoji: tip se upravo bira. Zato
//   konstruktor ne može biti virtual. Destruktor može i treba (EC++ Item 7).
// Ispravno: za "napravi kopiju pravog tipa" virtual clone() (main.cpp,
//   sekcija 9); za "napravi objekat po imenu tipa" fabrička funkcija.
class Widget {
public:
    virtual Widget() {}
};

int main() {}
