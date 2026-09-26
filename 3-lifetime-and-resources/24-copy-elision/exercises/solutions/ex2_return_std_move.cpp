// Rešenje zadatka ex2_return_std_move.

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

// Ako pišeš "return std::move(t);" (nije dobro): NRVO se isključi, pa
// umesto nula dobiješ jedan move.
// Treba ovako: "return t;" -- NRVO, a gde nije moguć, automatski move.
Text make() {
    Text t("result");
    return t;
}

int main() {
    Text r = make();
    std::cout << r.s << " -- copies: " << counter.copies << ", moves: " << counter.moves << '\n';
}
