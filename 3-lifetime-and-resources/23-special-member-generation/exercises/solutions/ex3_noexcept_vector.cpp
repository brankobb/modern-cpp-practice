// Rešenje zadatka ex3_noexcept_vector.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Counter {
    int copies = 0;
    int moves = 0;
} counter;

// Ako move konstruktor nije noexcept (nije dobro): vector pri rastu kopira
// sve elemente, jer samo tako može da zadrži jaku garanciju.
// Treba ovako: noexcept na move operacijama (C.66). Obećanje mora da bude
// tačno -- ako noexcept funkcija ipak baci, poziva se std::terminate.
struct Sample {
    std::string name;
    explicit Sample(std::string n) : name(std::move(n)) {}
    Sample(const Sample& o) : name(o.name) { ++counter.copies; }
    Sample(Sample&& o) noexcept : name(std::move(o.name)) { ++counter.moves; }
};
static_assert(std::is_nothrow_move_constructible_v<Sample>);

int main() {
    std::vector<Sample> v;
    for (int i = 0; i < 20; ++i) v.emplace_back("sample-" + std::to_string(i));
    std::cout << "copies during growth: " << counter.copies << '\n';
    std::cout << "moves during growth: " << (counter.moves > 0 ? "yes" : "no") << '\n';
}
