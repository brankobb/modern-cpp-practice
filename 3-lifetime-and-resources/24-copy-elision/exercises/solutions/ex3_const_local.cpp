// Rešenje zadatka ex3_const_local.

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

// Ako su lokalne koje vraćaš const (nije dobro): automatski move na
// return-u ne može da ih pomeri, pa ih kopira.
// Treba ovako: lokalna koju vraćaš nije const. Kad NRVO nije moguć (dve
// različite lokalne), dobiješ move.
Text choose(bool isShort) {
    Text a("short");
    Text b("long");
    if (isShort) return a;
    return b;
}

int main() {
    Text r = choose(true);
    std::cout << r.s << " -- copies: " << counter.copies << ", moves: " << counter.moves << '\n';
}
