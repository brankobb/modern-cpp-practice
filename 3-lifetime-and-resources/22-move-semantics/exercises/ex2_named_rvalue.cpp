// KIND: why
// DEMO-OUT: NAIVE copies: 1, moves: 0
//
// Zadatak 2 -- zašto parametar T&& unutar funkcije treba std::move
// (sekcija 5)
// Rešenje: exercises/solutions/ex2_named_rvalue.cpp
//
// Message ima konstruktor koji prima Text&& -- "daj mi privremeni, ja ću
// ga preuzeti".
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/22-move-semantics/exercises/ex2_named_rvalue.cpp -DNAIVE
//   Iako je argument privremeni i parametar je Text&&, član se KOPIRA.
//   Parametar t ima ime, pa je izraz "t" lvalue (kategorija vrednosti je
//   osobina IZRAZA, ne tipa). Kompajler ne sme sam da ga pomeri -- mogao
//   bi da se koristi i posle, u istom telu.
// Korak 2: u #else grani napiši konstruktor sa t_(std::move(t)).
// Korak 3: dodaj i konstruktor Message(const Text& t) za lvalue argumente
//   i proveri: lvalue -> 1 kopija, privremeni -> 1 pomeranje. (lekcija 24
//   pokazuje kraću varijantu: jedan konstruktor koji prima PO VREDNOSTI.)

#include <iostream>
#include <string>
#include <utility>

struct Counter {
    int copies = 0;
    int moves = 0;
} counter;

struct Text {
    std::string s;
    explicit Text(std::string x) : s(std::move(x)) {}
    Text(const Text& o) : s(o.s) { ++counter.copies; }
    Text(Text&& o) noexcept : s(std::move(o.s)) { ++counter.moves; }
};

#ifdef NAIVE
struct Message {
    explicit Message(Text&& t) : t_(t) {}      // t je ovde lvalue!
    Text t_;
};

int main() {
    Message m(Text("hello"));
    std::cout << "copies: " << counter.copies << ", moves: " << counter.moves << '\n';
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // Message m(Text("hello"));
    // std::cout << "temporary -> copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    // Text t("named");
    // Message n(t);
    // std::cout << "lvalue -> copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    // std::cout << m.t_.s << ' ' << n.t_.s << ' ' << t.s << '\n';
}
#endif

/* EXPECTED OUTPUT
temporary -> copies: 0, moves: 1
lvalue -> copies: 1, moves: 1
hello named named
*/
