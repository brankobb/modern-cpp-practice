// EXPECT-GCC: use of deleted function 'std::variant<_Types>::variant() [with _Types = {Senzor, int}]'
// EXPECT-CLANG: call to implicitly-deleted default constructor of 'std::variant<Senzor, int>'
// POGREŠNO: podrazumevani variant drži podrazumevano napravljenu PRVU
// alternativu. Senzor nema podrazumevani konstruktor, pa ni variant.
// Ispravno: std::variant<std::monostate, Senzor, int> -- monostate je
// "prazno" stanje na prvom mestu; ili odmah vrednost: v = 5 / v{Senzor{1}}.
#include <variant>
struct Senzor {
    explicit Senzor(int) {}
};
int main() {
    std::variant<Senzor, int> v;
    return static_cast<int>(v.index());
}
