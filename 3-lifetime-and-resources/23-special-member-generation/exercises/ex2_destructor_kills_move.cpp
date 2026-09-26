// KIND: why
// DEMO-OUT: NAIVE copies: 1, moves: 0
//
// Zadatak 2 -- zašto "samo dodajem destruktor za log" menja performanse
// (sekcija 2)
// Rešenje: exercises/solutions/ex2_destructor_kills_move.cpp
//
// Message ima član Text (koji broji kopije i pomeranja). Neko je dodao
// destruktor da loguje uništavanje.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/23-special-member-generation/exercises/ex2_destructor_kills_move.cpp -DNAIVE
//   std::move(p) KOPIRA. Korisnički destruktor ukida generisanje move
//   operacija (tabela, sekcija 1), pa poziv ide na copy konstruktor. Nema
//   greške ni upozorenja (-Wdeprecated-copy-dtor nije u -Wall -Wextra).
// Korak 2: u #else grani zadrži destruktor, ali vrati move: deklariši
//   svih pet specijalnih funkcija kao = default (rule of 5). Pazi: čim
//   deklarišeš move, kopija se BRIŠE -- zato i nju = default. U C++20
//   Message tada više nije agregat (bilo koji deklarisan konstruktor, i
//   = default), pa Message{Text(...)} traži konstruktor
//   explicit Message(Text x); u C++17 bi radilo i bez njega.
// Korak 3: (za razmišljanje) kad destruktor ne radi ništa korisno, najbolje
//   je da ga nema (rule of 0). Zašto log u destruktoru često nije vredan
//   ove cene?

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
    Text& operator=(const Text&) = default;
    Text& operator=(Text&&) noexcept = default;
};

int destroyed = 0;

#ifdef NAIVE
struct Message {
    Text t;
    ~Message() { ++destroyed; }          // "samo log"
};
#else
struct Message {
    Text t;
    // TODO korak 2
};
#endif

int main() {
    {
        Message p{Text("content")};
        counter = Counter{};               // brojimo samo ono što uradi std::move
        Message q = std::move(p);
        (void)q;
        std::cout << "copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    }
#ifndef NAIVE
    // Korak 2 -- otkomentariši:
    // std::cout << "destroyed: " << destroyed << '\n';
#endif
}

/* EXPECTED OUTPUT
copies: 0, moves: 1
destroyed: 2
*/
