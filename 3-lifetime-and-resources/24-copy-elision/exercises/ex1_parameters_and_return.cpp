// KIND: usage
//
// Zadatak 1 -- vraćanje po vrednosti, sink parametar, emplace_back
// (sekcije 1, 3, 4)
//   ./build.sh 3-lifetime-and-resources/24-copy-elision/exercises/ex1_parameters_and_return.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_parameters_and_return.cpp
//
// Text broji kopije i pomeranja (i konstruktorom i dodelom).
// Korak 1: Text make(const char* s) -- vrati PRVALUE: return Text(s);
//   i Text makeNamed(const char* s) -- Text t(s); return t;
//   Prvi je garantovana elizija (C++17), drugi NRVO (dozvoljen, ne
//   garantovan -- ali g++ i clang ga rade i na -O0).
// Korak 2: class Device sa std::string-olikim članom Text name_ i
//   "sink" metodom void setName(Text name) { name_ = std::move(name); }
//   -- JEDNA funkcija, prima po vrednosti (F.18). Za lvalue argument:
//   1 kopija (u parametar) + 1 move (u član); za rvalue: 2 move-a.
// Korak 3: std::vector<Text> v; v.reserve(2); pa v.push_back(Text("a"))
//   i v.emplace_back("b"). emplace_back prosledi argumente konstruktoru i
//   napravi element NA MESTU -- nema privremenog objekta.

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

// TODO korak 1 i 2

int main() {
    // Korak 1 -- otkomentariši:
    // Text a = make("a");
    // counter.print("prvalue");
    // Text b = makeNamed("b");
    // counter.print("NRVO");

    // Korak 2 -- otkomentariši:
    // Device u;
    // u.setName(a);
    // counter.print("sink, lvalue");
    // u.setName(std::move(b));
    // counter.print("sink, rvalue");

    // Korak 3 -- otkomentariši:
    // std::vector<Text> v;
    // v.reserve(2);
    // v.push_back(Text("c"));
    // counter.print("push_back");
    // v.emplace_back("d");
    // counter.print("emplace_back");
}

/* EXPECTED OUTPUT
prvalue: copies 0, moves 0
NRVO: copies 0, moves 0
sink, lvalue: copies 1, moves 1
sink, rvalue: copies 0, moves 2
push_back: copies 0, moves 1
emplace_back: copies 0, moves 0
*/
