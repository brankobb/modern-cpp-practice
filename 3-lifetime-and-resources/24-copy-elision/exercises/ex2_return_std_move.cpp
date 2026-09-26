// KIND: why
// DEMO-OUT: NAIVE copies: 0, moves: 1
//
// Zadatak 2 -- zašto NE pisati return std::move(lokalna) (sekcije 1, 2)
// Rešenje: exercises/solutions/ex2_return_std_move.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/24-copy-elision/exercises/ex2_return_std_move.cpp -DNAIVE
//   Ideja "pomeriću je da se ne kopira" daje GORI rezultat: 1 move umesto
//   0. std::move(t) nije ime lokalne promenljive nego izraz tipa Text&&,
//   pa NRVO više ne može da se primeni -- ostaje samo move. Pročitaj i
//   upozorenje: g++ i clang (-Wpessimizing-move, u -Wall) ga daju.
// Korak 2: u #else grani napiši make() sa običnim return t;
//   Ako NRVO nije moguć, kompajler sam uradi move (sekcija 2) -- std::move
//   na return-u lokalne nikad ne pomaže.

#include <iostream>
#include <utility>

struct Counter {
    int copies = 0;
    int moves = 0;
} counter;

struct Text {
    const char* s;
    explicit Text(const char* x) : s(x) {}
    Text(const Text& o) : s(o.s) { ++counter.copies; }
    Text(Text&& o) noexcept : s(o.s) { ++counter.moves; }
};

#ifdef NAIVE
Text make() {
    Text t("result");
    return std::move(t);
}
#else
// TODO korak 2 (dok ne napišeš, ovo vraća prvalue)
Text make() { return Text("result"); }
#endif

int main() {
    Text r = make();
    std::cout << r.s << " -- copies: " << counter.copies << ", moves: " << counter.moves << '\n';
}

/* EXPECTED OUTPUT
result -- copies: 0, moves: 0
*/
