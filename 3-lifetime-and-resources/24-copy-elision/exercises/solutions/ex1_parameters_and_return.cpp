// Rešenje zadatka ex1_parameters_and_return.

#include <iostream>
#include <utility>
#include <vector>

struct Counter {
    int copies = 0;
    int moves = 0;
    void print(const char* label) {
        std::cout << label << ": copies " << copies << ", moves " << moves << '\n';
        *this = Counter{};
    }
} counter;

struct Text {
    const char* s;
    explicit Text(const char* x) : s(x) {}
    Text(const Text& o) : s(o.s) { ++counter.copies; }
    Text(Text&& o) noexcept : s(o.s) { ++counter.moves; }
    Text& operator=(const Text& o) {
        s = o.s;
        ++counter.copies;
        return *this;
    }
    Text& operator=(Text&& o) noexcept {
        s = o.s;
        ++counter.moves;
        return *this;
    }
};

// Korak 1: return prvalue -- objekat se pravi direktno u odredištu
// (C++17 garantovano, i za tipove bez kopije i move-a).
Text make(const char* s) { return Text(s); }

// NRVO: lokalna promenljiva se pravi direktno u odredištu. Nije
// garantovano, ali kad ga nema, return lokalne je automatski move.
Text makeNamed(const char* s) {
    Text t(s);
    return t;
}

// Korak 2: jedan "sink" po vrednosti umesto para const& / && overload-a.
// Cena: jedan move više nego sa dva overload-a -- obično zanemarljivo.
class Device {
public:
    void setName(Text name) { name_ = std::move(name); }

private:
    Text name_{""};
};

int main() {
    Text a = make("a");
    counter.print("prvalue");
    Text b = makeNamed("b");
    counter.print("NRVO");

    Device u;
    u.setName(a);
    counter.print("sink, lvalue");
    u.setName(std::move(b));
    counter.print("sink, rvalue");

    std::vector<Text> v;
    v.reserve(2);
    v.push_back(Text("c"));
    counter.print("push_back");
    v.emplace_back("d");
    counter.print("emplace_back");
}
