// VRSTA: zašto
// DEMO-OUT: NAIVNO port 0: zdravo
//
// Zadatak 3 -- zašto konstruktor baca, umesto init() koji vraća bool
// (sekcija 7)
// Rešenje: exercises/solutions/z3_konstruktor_baca.cpp
//
// Uređaj ima portove 0..3.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week3-advanced/s11-exceptions/exercises/z3_konstruktor_baca.cpp -DNAIVNO
//   "Dvofazna inicijalizacija": Port se napravi prazan, pa se otvori
//   pozivom otvori(), koji vraća false za nepostojeći port. Pozivalac je
//   zaboravio da proveri rezultat -- i poruka je tiho otišla na port 0.
//   Objekat postoji, a nije ispravan: SVAKA metoda mora da proverava
//   "da li sam otvoren", i svaki pozivalac mora da proverava otvori().
// Korak 2: u #else grani: explicit Port(int broj) koji baca
//   std::invalid_argument("port <broj> ne postoji") za broj van 0..3.
//   Ako konstruktor baci, objekat NE POSTOJI -- pa posalji() ne mora ništa
//   da proverava (invarijanta: svaki Port je otvoren).
// Korak 3: (možeš i ovako, kad izuzeci nisu dozvoljeni -- npr. embedded sa
//   -fno-exceptions) fabrika koja vraća std::optional<Port>, i
//   [[nodiscard]] na njoj. Razmisli: šta je tu "zaboravljena provera"?

#include <iostream>
#include <stdexcept>
#include <string>

#ifdef NAIVNO
class Port {
public:
    bool otvori(int broj) {
        if (broj < 0 || broj > 3) return false;
        broj_ = broj;
        return true;
    }
    void posalji(const char* s) const { std::cout << "port " << broj_ << ": " << s << '\n'; }

private:
    int broj_ = 0;
};

int main() {
    Port p;
    p.otvori(9);             // rezultat ignorisan
    p.posalji("zdravo");
}
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // try {
    //     Port p(9);
    //     p.posalji("zdravo");
    // } catch (const std::invalid_argument& e) {
    //     std::cout << "greška: " << e.what() << '\n';
    // }
    // Port ok(2);
    // ok.posalji("zdravo");
}
#endif

/* OČEKIVANI IZLAZ
greška: port 9 ne postoji
port 2: zdravo
*/
