// KIND: why
// DEMO-OUT: NAIVE iste konekcije: 1
// DEMO-ERR: EXPLICIT no match for 'operator=='|invalid operands to binary expression
//
// Zadatak 2 -- zašto explicit operator bool (sekcija 6, C.164)
// Rešenje: exercises/solutions/ex2_explicit_bool.cpp
//
// Konekcija ima operator bool: "da li je otvorena", da bi radilo
// if (k) { ... }.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/17-type-conversions/exercises/ex2_explicit_bool.cpp -DNAIVE
//   Dve RAZLIČITE otvorene konekcije (port 80 i port 443) su "iste", a
//   k1 + 10 daje 11. Konekcija nema operator==, pa kompajler obe strane
//   pretvori u bool (true == true), a bool se dalje promoviše u int.
// Korak 2: isto sa explicit operator bool:
//     ./build.sh .../ex2_explicit_bool.cpp -DEXPLICIT
//   k1 == k2 više nije moguće. if (k1) i dalje radi -- if je "kontekst
//   bool-a", gde je explicit konverzija dozvoljena.
// Korak 3: u #else grani napiši Konekciju sa explicit operator bool i
//   pravim operator== koji poredi port. Otkomentariši test.

#include <iostream>

#if defined(NAIVE) || defined(EXPLICIT)
class Konekcija {
public:
    explicit Konekcija(int port) : port_(port) {}
#ifdef EXPLICIT
    explicit
#endif
    operator bool() const { return port_ != 0; }
private:
    int port_;
};

int main() {
    Konekcija k1(80), k2(443);
    if (k1) std::cout << "k1 otvorena\n";
    std::cout << "iste konekcije: " << (k1 == k2) << '\n';
    std::cout << "k1 + 10 = " << k1 + 10 << '\n';
}
#else
// TODO korak 3

int main() {
    // Korak 3 -- otkomentariši:
    // Konekcija k1(80), k2(443), k3(80), zatvorena(0);
    // if (k1) std::cout << "k1 otvorena\n";
    // if (!zatvorena) std::cout << "zatvorena nije otvorena\n";
    // std::cout << "k1 == k2: " << (k1 == k2) << ", k1 == k3: " << (k1 == k3) << '\n';
    // bool b = static_cast<bool>(k2);   // "bool b = k2;" ne bi radilo
    // std::cout << "k2 kao bool: " << b << '\n';
}
#endif

/* EXPECTED OUTPUT
k1 otvorena
zatvorena nije otvorena
k1 == k2: 0, k1 == k3: 1
k2 kao bool: 1
*/
