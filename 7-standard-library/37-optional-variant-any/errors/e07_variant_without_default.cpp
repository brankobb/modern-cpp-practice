// EXPECT-GCC: use of deleted function 'std::variant<_Types>::variant() [with _Types = {Sensor, int}]'
// EXPECT-CLANG: call to implicitly-deleted default constructor of 'std::variant<Sensor, int>'
// POGREŠNO: podrazumevani variant drži podrazumevano napravljenu PRVU
// alternativu. Sensor nema podrazumevani konstruktor, pa ni variant.
// Ispravno: std::variant<std::monostate, Sensor, int> -- monostate je
// "prazno" stanje na prvom mestu; ili odmah vrednost: v = 5 / v{Sensor{1}}.
#include <variant>
struct Sensor {
    explicit Sensor(int) {}
};
int main() {
    std::variant<Sensor, int> v;
    return static_cast<int>(v.index());
}
