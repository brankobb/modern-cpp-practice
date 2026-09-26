// Rešenje zadatka ex1_perfect_forwarding.

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

// Korak 1: jedna funkcija za oba slučaja. Bez std::forward bi x (ima ime)
// uvek bio lvalue, pa bi i rvalue bio kopiran; sa std::move bi i lvalue
// bio pomeren (ex2).
template <typename T>
void add(std::vector<Text>& v, T&& x) {
    v.push_back(std::forward<T>(x));
}

// Korak 2: svaki argument prosleđen sa svojom kategorijom. Brojke za
// rvalue: 1 move u parametar Device-a + 1 move u član = 2; za lvalue:
// kopija u parametar + move u član.
template <typename T, typename... Args>
std::unique_ptr<T> make(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Korak 3: omotač koji "ne postoji" za argumente -- sve prosledi dalje
// po referenci, sa istom kategorijom. decltype(auto) bi čuvao i referencu
// u povratnom tipu; ovde je dovoljno auto.
template <typename F, typename... Args>
auto measure(F&& f, Args&&... args) {
    std::cout << "call\n";
    return std::forward<F>(f)(std::forward<Args>(args)...);
}

int main() {
    std::vector<Text> v;
    v.reserve(2);
    Text a("a");
    add(v, a);
    counter.print("add lvalue");
    add(v, Text("b"));
    counter.print("add rvalue");

    auto u = make<Device>(Text("motor"), 7);
    counter.print("make, rvalue");
    auto w = make<Device>(a, 8);
    counter.print("make, lvalue");
    std::cout << u->name.s << ' ' << u->id << ", " << w->name.s << ' ' << w->id << '\n';

    std::size_t n = measure(length, a);
    std::cout << "length: " << n << '\n';
    counter.print("measure");
}
