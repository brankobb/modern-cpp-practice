// KIND: usage
//
// Zadatak 1 -- forwarding reference, std::forward i variadic template
// (sekcije 1, 3)
//   ./build.sh 4-templates/27-forwarding-and-lifetime/exercises/ex1_perfect_forwarding.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_perfect_forwarding.cpp
//
// Text broji kopije i pomeranja.
// Korak 1: template <typename T> void add(std::vector<Text>& v, T&& x)
//   -- ubaci x u vektor tako da se lvalue KOPIRA, a rvalue POMERI:
//   v.push_back(std::forward<T>(x)). T&& u template-u je forwarding
//   referenca: T pamti da li je argument bio lvalue (T = Text&) ili
//   rvalue (T = Text).
// Korak 2: template <typename T, typename... Args>
//   std::unique_ptr<T> make(Args&&... args) -- sopstveni make_unique:
//   return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
//   Device(Text name, int id) prima Text po vrednosti.
// Korak 3: template <typename F, typename... Args>
//   auto measure(F&& f, Args&&... args) -- ispiše "call" i vrati rezultat
//   std::forward<F>(f)(std::forward<Args>(args)...). Omotač ne sme da
//   doda nijednu kopiju.

#include <iostream>
#include <memory>
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
};

struct Device {
    Device(Text i, int num) : name(std::move(i)), id(num) {}
    Text name;
    int id;
};

std::size_t length(const Text& t) {
    std::size_t n = 0;
    while (t.s[n]) ++n;
    return n;
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // std::vector<Text> v;
    // v.reserve(2);
    // Text a("a");
    // add(v, a);
    // counter.print("add lvalue");
    // add(v, Text("b"));
    // counter.print("add rvalue");

    // Korak 2 -- otkomentariši:
    // auto u = make<Device>(Text("motor"), 7);
    // counter.print("make, rvalue");
    // auto w = make<Device>(a, 8);
    // counter.print("make, lvalue");
    // std::cout << u->name.s << ' ' << u->id << ", " << w->name.s << ' ' << w->id << '\n';

    // Korak 3 -- otkomentariši:
    // std::size_t n = measure(length, a);
    // std::cout << "length: " << n << '\n';
    // counter.print("measure");
}

/* EXPECTED OUTPUT
add lvalue: copies 1, moves 0
add rvalue: copies 0, moves 1
make, rvalue: copies 0, moves 2
make, lvalue: copies 1, moves 1
motor 7, a 8
call
length: 1
measure: copies 0, moves 0
*/
