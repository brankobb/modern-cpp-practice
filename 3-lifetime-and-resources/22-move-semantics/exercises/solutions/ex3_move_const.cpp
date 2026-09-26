// Rešenje zadatka ex3_move_const.

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
    // Ako je promenljiva const, a posle je pomeraš (nije dobro): std::move
    // da const Text&&, move konstruktor ne može da ga primi, i dobije se
    // kopija -- bez ikakvog upozorenja.
    // Treba ovako: objekat koji ćeš pomeriti nije const (EMC Item 23:
    // "ne deklariši objekte const ako želiš da ih pomeriš").
    Text message("a long message that goes into the queue");
    queue.push_back(std::move(message));
    std::cout << "copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    // Moved-from std::string je u "validnom, ali nespecifikovanom stanju".
    // libstdc++ ga ostavi praznim (provereno); standard to ne garantuje, pa
    // se na to ne oslanjaj u pravom kodu. (Ako na Windows-u/libc++ vidiš
    // nešto drugo, javi.)
    std::cout << "message after: \"" << message.s << "\"\n";
}
