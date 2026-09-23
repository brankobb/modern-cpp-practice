// Rešenje zadatka z3_noexcept_vector.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

// Ako move konstruktor nije noexcept (nije dobro): vector pri rastu kopira
// sve elemente, jer samo tako može da zadrži jaku garanciju.
// Treba ovako: noexcept na move operacijama (C.66). Obećanje mora da bude
// tačno -- ako noexcept funkcija ipak baci, poziva se std::terminate.
struct Uzorak {
    std::string ime;
    explicit Uzorak(std::string i) : ime(std::move(i)) {}
    Uzorak(const Uzorak& o) : ime(o.ime) { ++brojac.kopija; }
    Uzorak(Uzorak&& o) noexcept : ime(std::move(o.ime)) { ++brojac.pomeranja; }
};
static_assert(std::is_nothrow_move_constructible_v<Uzorak>);

int main() {
    std::vector<Uzorak> v;
    for (int i = 0; i < 20; ++i) v.emplace_back("uzorak-" + std::to_string(i));
    std::cout << "kopija pri rastu: " << brojac.kopija << '\n';
    std::cout << "pomeranja pri rastu: " << (brojac.pomeranja > 0 ? "da" : "ne") << '\n';
}
