// KIND: why
// DEMO-OUT: NAIVE port 0: hello
//
// Zadatak 3 -- zašto konstruktor baca, umesto init() koji vraća bool
// (sekcija 7)
// Rešenje: exercises/solutions/ex3_constructor_throws.cpp
//
// Uređaj ima portove 0..3.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/18-exceptions/exercises/ex3_constructor_throws.cpp -DNAIVE
//   "Dvofazna inicijalizacija": Port se napravi prazan, pa se otvori
//   pozivom open(), koji vraća false za nepostojeći port. Pozivalac je
//   zaboravio da proveri rezultat -- i poruka je tiho otišla na port 0.
//   Objekat postoji, a nije ispravan: SVAKA metoda mora da proverava
//   "da li sam otvoren", i svaki pozivalac mora da proverava open().
// Korak 2: u #else grani: explicit Port(int number) koji baca
//   std::invalid_argument("port <number> does not exist") za broj van 0..3.
//   Ako konstruktor baci, objekat NE POSTOJI -- pa send() ne mora ništa
//   da proverava (invarijanta: svaki Port je otvoren).
// Korak 3: (možeš i ovako, kad izuzeci nisu dozvoljeni -- npr. embedded sa
//   -fno-exceptions) fabrika koja vraća std::optional<Port>, i
//   [[nodiscard]] na njoj. Razmisli: šta je tu "zaboravljena provera"?

#include <iostream>
#include <stdexcept>
#include <string>

#ifdef NAIVE
class Port {
public:
    bool open(int number) {
        if (number < 0 || number > 3) return false;
        number_ = number;
        return true;
    }
    void send(const char* s) const { std::cout << "port " << number_ << ": " << s << '\n'; }

private:
    int number_ = 0;
};

int main() {
    Port p;
    p.open(9);               // rezultat ignorisan
    p.send("hello");
}
#else
// TODO korak 2

int main() {
    // Korak 2 -- otkomentariši:
    // try {
    //     Port p(9);
    //     p.send("hello");
    // } catch (const std::invalid_argument& e) {
    //     std::cout << "error: " << e.what() << '\n';
    // }
    // Port ok(2);
    // ok.send("hello");
}
#endif

/* EXPECTED OUTPUT
error: port 9 does not exist
port 2: hello
*/
