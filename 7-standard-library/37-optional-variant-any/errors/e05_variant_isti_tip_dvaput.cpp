// EXPECT-GCC: T must occur exactly once in alternatives
// EXPECT-CLANG: T must occur exactly once in alternatives
// POGREŠNO: variant<int, int> je dozvoljen, ali get<int> je dvosmislen --
// koji od dva int-a? Pristup po tipu radi samo za tip koji se javlja
// tačno jednom.
// Ispravno: po indeksu, std::get<0>(v); ili različiti tipovi
// (struct Celzijus { int v; }; struct Farenhajt { int v; };).
#include <variant>
int main() {
    std::variant<int, int> v;
    return std::get<int>(v);
}
