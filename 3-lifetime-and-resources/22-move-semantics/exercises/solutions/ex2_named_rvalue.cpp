// Rešenje zadatka ex2_named_rvalue.

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

// Ako pišeš t_(t) za parametar Text&& t (nije dobro): t ima ime, pa je
// lvalue -- poziva se copy konstruktor.
// Treba ovako: std::move(t) -- "ovde mi t više ne treba, sme da se pomeri".
struct Message {
    explicit Message(Text&& t) : t_(std::move(t)) {}
    explicit Message(const Text& t) : t_(t) {}          // lvalue: kopija je ispravna
    Text t_;
};

int main() {
    Message m(Text("hello"));
    std::cout << "temporary -> copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    Text t("named");
    Message n(t);
    std::cout << "lvalue -> copies: " << counter.copies << ", moves: " << counter.moves << '\n';
    std::cout << m.t_.s << ' ' << n.t_.s << ' ' << t.s << '\n';
}
