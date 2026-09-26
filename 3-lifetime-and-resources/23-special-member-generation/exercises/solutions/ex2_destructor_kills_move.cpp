// Rešenje zadatka ex2_destructor_kills_move.

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

// Ako dodaš samo destruktor (nije dobro): move operacije se ne generišu,
// i svaki std::move tiho postane kopija.
// Treba ovako: kad deklarišeš bilo koju od pet, deklariši svih pet (C.21).
// = default zadrži ponašanje "član po član", a noexcept se izvede iz
// članova.
struct Message {
    Text t;
    explicit Message(Text x) : t(std::move(x)) {}
    ~Message() { ++destroyed; }
    Message(const Message&) = default;
    Message& operator=(const Message&) = default;
    Message(Message&&) = default;
    Message& operator=(Message&&) = default;
};

int main() {
    {
        Message p{Text("content")};
        counter = Counter{};               // brojimo samo ono što uradi std::move
        Message q = std::move(p);
        (void)q;
        std::cout << "copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    }
    std::cout << "destroyed: " << destroyed << '\n';
}
