// KIND: why
// DEMO-OUT: NAIVNO kopija: 1, pomeranja: 0
//
// Zadatak 3 -- zašto std::move na const objektu tiho kopira
// (sekcija 3, EMC Item 23)
// Rešenje: exercises/solutions/ex3_move_const.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/22-move-semantics/exercises/ex3_move_const.cpp -DNAIVNO
//   poruka je const, pa je std::move(poruka) tipa const Tekst&&. Move
//   konstruktor prima Tekst&& (ne-const) -- ne može da se veže. Zato
//   pobedi copy konstruktor (const Tekst& prima i const rvalue).
//   Nema greške ni upozorenja sa -Wall -Wextra: "move" je tiho postao kopija.
// Korak 2: u #else grani ukloni const sa promenljive koju nameravaš da
//   pomeriš, i proveri brojače.
// Korak 3: (za razmišljanje) zašto move konstruktor ne prima const Tekst&&?
//   (Odgovor: da bi ukrao sadržaj, mora da izmeni izvor -- ostavi ga
//   praznim. Iz const objekta ne može.)

#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

struct Tekst {
    std::string s;
    explicit Tekst(std::string x) : s(std::move(x)) {}
    Tekst(const Tekst& o) : s(o.s) { ++brojac.kopija; }
    Tekst(Tekst&& o) noexcept : s(std::move(o.s)) { ++brojac.pomeranja; }
};

int main() {
    std::vector<Tekst> red;
    red.reserve(1);
#ifdef NAIVNO
    const Tekst poruka("dugačka poruka koja se šalje u red");
    red.push_back(std::move(poruka));
    std::cout << "kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
#else
    // TODO korak 2
    // Korak 2 -- otkomentariši:
    // red.push_back(std::move(poruka));
    // std::cout << "kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    // std::cout << "poruka posle: \"" << poruka.s << "\"\n";
#endif
}

/* EXPECTED OUTPUT
kopija: 0, pomeranja: 1
poruka posle: ""
*/
