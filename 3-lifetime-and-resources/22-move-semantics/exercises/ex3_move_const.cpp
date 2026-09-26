// KIND: why
// DEMO-OUT: NAIVE copies: 1, moves: 0
//
// Zadatak 3 -- zašto std::move na const objektu tiho kopira
// (sekcija 3, EMC Item 23)
// Rešenje: exercises/solutions/ex3_move_const.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/22-move-semantics/exercises/ex3_move_const.cpp -DNAIVE
//   message je const, pa je std::move(message) tipa const Text&&. Move
//   konstruktor prima Text&& (ne-const) -- ne može da se veže. Zato
//   pobedi copy konstruktor (const Text& prima i const rvalue).
//   Nema greške ni upozorenja sa -Wall -Wextra: "move" je tiho postao kopija.
// Korak 2: u #else grani ukloni const sa promenljive koju nameravaš da
//   pomeriš, i proveri brojače.
// Korak 3: (za razmišljanje) zašto move konstruktor ne prima const Text&&?
//   (Odgovor: da bi ukrao sadržaj, mora da izmeni izvor -- ostavi ga
//   praznim. Iz const objekta ne može.)

#include <iostream>
#include <string>
#include <utility>
#include <vector>

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

int main() {
    std::vector<Text> queue;
    queue.reserve(1);
#ifdef NAIVE
    const Text message("a long message that goes into the queue");
    queue.push_back(std::move(message));
    std::cout << "copies: " << counter.copies << ", moves: " << counter.moves << '\n';
#else
    // TODO korak 2
    // Korak 2 -- otkomentariši:
    // queue.push_back(std::move(message));
    // std::cout << "copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    // std::cout << "message after: \"" << message.s << "\"\n";
#endif
}

/* EXPECTED OUTPUT
copies: 0, moves: 1
message after: ""
*/
