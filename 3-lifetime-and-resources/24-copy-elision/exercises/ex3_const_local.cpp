// KIND: why
// DEMO-OUT: NAIVE copies: 1, moves: 0
//
// Zadatak 3 -- zašto lokalna koju vraćaš ne treba da bude const (sekcija 2)
// Rešenje: exercises/solutions/ex3_const_local.cpp
//
// choose() vraća jednu od dve lokalne, zavisno od uslova -- NRVO tada
// nije moguć (kompajler ne zna unapred koju da napravi u odredištu).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/24-copy-elision/exercises/ex3_const_local.cpp -DNAIVE
//   Lokalne su const ("ne menjam ih, pa neka budu const"). Automatski
//   move na return-u tretira lokalnu kao rvalue: const Text&& -- move
//   konstruktor ga ne prima, pa je KOPIJA (isti razlog kao lekcija 22 ex3).
// Korak 2: u #else grani ukloni const sa lokalnih. Sada je 1 move.
// Korak 3: (za razmišljanje) kada je const lokalna u redu? (Kad je ne
//   vraćaš i ne pomeraš -- tada const samo pomaže.)

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
Text choose(bool isShort) {
    const Text a("short");
    const Text b("long");
    if (isShort) return a;
    return b;
}
#else
// TODO korak 2 (dok ne napišeš, ovo vraća prvalue)
Text choose(bool isShort) { return Text(isShort ? "short" : "long"); }
#endif

int main() {
    Text r = choose(true);
    std::cout << r.s << " -- copies: " << counter.copies << ", moves: " << counter.moves << '\n';
}

/* EXPECTED OUTPUT
short -- copies: 0, moves: 1
*/
