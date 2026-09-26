// KIND: why
// DEMO-OUT: NAIVE sending packet of size 42
// DEMO-ERR: EXPLICIT could not convert|invalid initialization|no viable conversion|no matching function
//
// Zadatak 3 -- zašto explicit na konstruktoru sa jednim argumentom
// (sekcija 2, C.46)
// Rešenje: exercises/solutions/ex3_explicit.cpp
//
// Packet(std::size_t size) pravi prazan paket date veličine.
// send(const Packet&) šalje paket.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/14-class-basics/exercises/ex3_explicit.cpp -DNAIVE
//   Programer je hteo da pošalje BAJT 42, a napisao send(42). Kompajler
//   je tiho napravio privremeni Packet od 42 prazna bajta (konstruktor sa
//   jednim argumentom je i implicitna konverzija size_t -> Packet).
// Korak 2: isto sa explicit konstruktorom:
//     ./build.sh .../ex3_explicit.cpp -DEXPLICIT
//   Sada je greška, a namera mora da se napiše: send(Packet(42)).
// Korak 3: u #else grani napiši Packet sa explicit Packet(std::size_t) i
//   drugim konstruktorom Packet(std::initializer_list<unsigned char>) koji
//   pravi paket sa tim bajtovima (njega NE treba da bude explicit, da bi
//   send({0x42}) radilo). Otkomentariši test.

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <vector>

#if defined(NAIVE) || defined(EXPLICIT)
class Packet {
public:
#ifdef EXPLICIT
    explicit
#endif
    Packet(std::size_t size) : bytes_(size) {}
    std::size_t size() const { return bytes_.size(); }
private:
    std::vector<unsigned char> bytes_;
};

void send(const Packet& p) { std::cout << "sending packet of size " << p.size() << '\n'; }

int main() { send(42); }
#else
// TODO korak 3

int main() {
    // Korak 3 -- otkomentariši:
    // send(Packet(3));
    // send({0x42});
    // send({0x01, 0x02});
}
#endif

/* EXPECTED OUTPUT
sending packet of size 3: 00 00 00
sending packet of size 1: 42
sending packet of size 2: 01 02
*/
