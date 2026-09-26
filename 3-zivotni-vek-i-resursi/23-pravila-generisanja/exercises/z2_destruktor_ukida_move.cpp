// VRSTA: zašto
// DEMO-OUT: NAIVNO kopija: 1, pomeranja: 0
//
// Zadatak 2 -- zašto "samo dodajem destruktor za log" menja performanse
// (sekcija 2)
// Rešenje: exercises/solutions/z2_destruktor_ukida_move.cpp
//
// Poruka ima član Tekst (koji broji kopije i pomeranja). Neko je dodao
// destruktor da loguje uništavanje.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-zivotni-vek-i-resursi/23-pravila-generisanja/exercises/z2_destruktor_ukida_move.cpp -DNAIVNO
//   std::move(p) KOPIRA. Korisnički destruktor ukida generisanje move
//   operacija (tabela, sekcija 1), pa poziv ide na copy konstruktor. Nema
//   greške ni upozorenja (-Wdeprecated-copy-dtor nije u -Wall -Wextra).
// Korak 2: u #else grani zadrži destruktor, ali vrati move: deklariši
//   svih pet specijalnih funkcija kao = default (rule of 5). Pazi: čim
//   deklarišeš move, kopija se BRIŠE -- zato i nju = default. U C++20
//   Poruka tada više nije agregat (bilo koji deklarisan konstruktor, i
//   = default), pa Poruka{Tekst(...)} traži konstruktor
//   explicit Poruka(Tekst x); u C++17 bi radilo i bez njega.
// Korak 3: (za razmišljanje) kad destruktor ne radi ništa korisno, najbolje
//   je da ga nema (rule of 0). Zašto log u destruktoru često nije vredan
//   ove cene?

#include <iostream>
#include <string>
#include <utility>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

struct Tekst {
    std::string s;
    explicit Tekst(std::string x) : s(std::move(x)) {}
    Tekst(const Tekst& o) : s(o.s) { ++brojac.kopija; }
    Tekst(Tekst&& o) noexcept : s(std::move(o.s)) { ++brojac.pomeranja; }
    Tekst& operator=(const Tekst&) = default;
    Tekst& operator=(Tekst&&) noexcept = default;
};

int unistenih = 0;

#ifdef NAIVNO
struct Poruka {
    Tekst t;
    ~Poruka() { ++unistenih; }           // "samo log"
};
#else
struct Poruka {
    Tekst t;
    // TODO korak 2
};
#endif

int main() {
    {
        Poruka p{Tekst("sadržaj")};
        brojac = Brojac{};                 // brojimo samo ono što uradi std::move
        Poruka q = std::move(p);
        (void)q;
        std::cout << "kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    }
#ifndef NAIVNO
    // Korak 2 -- otkomentariši:
    // std::cout << "uništenih: " << unistenih << '\n';
#endif
}

/* OČEKIVANI IZLAZ
kopija: 0, pomeranja: 1
uništenih: 2
*/
