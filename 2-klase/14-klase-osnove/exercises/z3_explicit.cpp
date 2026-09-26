// VRSTA: zašto
// DEMO-OUT: NAIVNO šaljem paket od 42 bajta
// DEMO-ERR: EXPLICIT could not convert|invalid initialization|no viable conversion|no matching function
//
// Zadatak 3 -- zašto explicit na konstruktoru sa jednim argumentom
// (sekcija 2, C.46)
// Rešenje: exercises/solutions/z3_explicit.cpp
//
// Paket(std::size_t velicina) pravi prazan paket date veličine.
// posalji(const Paket&) šalje paket.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-klase/14-klase-osnove/exercises/z3_explicit.cpp -DNAIVNO
//   Programer je hteo da pošalje BAJT 42, a napisao posalji(42). Kompajler
//   je tiho napravio privremeni Paket od 42 prazna bajta (konstruktor sa
//   jednim argumentom je i implicitna konverzija size_t -> Paket).
// Korak 2: isto sa explicit konstruktorom:
//     ./build.sh .../z3_explicit.cpp -DEXPLICIT
//   Sada je greška, a namera mora da se napiše: posalji(Paket(42)).
// Korak 3: u #else grani napiši Paket sa explicit Paket(std::size_t) i
//   drugim konstruktorom Paket(std::initializer_list<unsigned char>) koji
//   pravi paket sa tim bajtovima (njega NE treba da bude explicit, da bi
//   posalji({0x42}) radilo). Otkomentariši test.

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <vector>

#if defined(NAIVNO) || defined(EXPLICIT)
class Paket {
public:
#ifdef EXPLICIT
    explicit
#endif
    Paket(std::size_t velicina) : bajtovi_(velicina) {}
    std::size_t velicina() const { return bajtovi_.size(); }
private:
    std::vector<unsigned char> bajtovi_;
};

void posalji(const Paket& p) { std::cout << "šaljem paket od " << p.velicina() << " bajta\n"; }

int main() { posalji(42); }
#else
// TODO korak 3

int main() {
    // Korak 3 -- otkomentariši:
    // posalji(Paket(3));
    // posalji({0x42});
    // posalji({0x01, 0x02});
}
#endif

/* OČEKIVANI IZLAZ
šaljem paket od 3 bajta: 00 00 00
šaljem paket od 1 bajta: 42
šaljem paket od 2 bajta: 01 02
*/
