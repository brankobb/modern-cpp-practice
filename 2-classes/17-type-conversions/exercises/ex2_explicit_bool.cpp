// KIND: why
// DEMO-OUT: NAIVE same connections: 1
// DEMO-ERR: EXPLICIT no match for 'operator=='|invalid operands to binary expression
//
// Zadatak 2 -- zašto explicit operator bool (sekcija 6, C.164)
// Rešenje: exercises/solutions/ex2_explicit_bool.cpp
//
// Connection ima operator bool: "da li je otvorena", da bi radilo
// if (c) { ... }.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 2-classes/17-type-conversions/exercises/ex2_explicit_bool.cpp -DNAIVE
//   Dve RAZLIČITE otvorene konekcije (port 80 i port 443) su "iste", a
//   c1 + 10 daje 11. Connection nema operator==, pa kompajler obe strane
//   pretvori u bool (true == true), a bool se dalje promoviše u int.
// Korak 2: isto sa explicit operator bool:
//     ./build.sh .../ex2_explicit_bool.cpp -DEXPLICIT
//   c1 == c2 više nije moguće. if (c1) i dalje radi -- if je "kontekst
//   bool-a", gde je explicit konverzija dozvoljena.
// Korak 3: u #else grani napiši Connection sa explicit operator bool i
//   pravim operator== koji poredi port. Otkomentariši test.

#include <iostream>

#if defined(NAIVE) || defined(EXPLICIT)
class Connection {
public:
    explicit Connection(int port) : port_(port) {}
#ifdef EXPLICIT
    explicit
#endif
    operator bool() const { return port_ != 0; }
private:
    int port_;
};

int main() {
    Connection c1(80), c2(443);
    if (c1) std::cout << "c1 open\n";
    std::cout << "same connections: " << (c1 == c2) << '\n';
    std::cout << "c1 + 10 = " << c1 + 10 << '\n';
}
#else
// TODO korak 3

int main() {
    // Korak 3 -- otkomentariši:
    // Connection c1(80), c2(443), c3(80), closed(0);
    // if (c1) std::cout << "c1 open\n";
    // if (!closed) std::cout << "closed is not open\n";
    // std::cout << "c1 == c2: " << (c1 == c2) << ", c1 == c3: " << (c1 == c3) << '\n';
    // bool b = static_cast<bool>(c2);   // "bool b = c2;" ne bi radilo
    // std::cout << "c2 as bool: " << b << '\n';
}
#endif

/* EXPECTED OUTPUT
c1 open
closed is not open
c1 == c2: 0, c1 == c3: 1
c2 as bool: 1
*/
