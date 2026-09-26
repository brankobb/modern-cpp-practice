// EXPECT-GCC: no type named 'type' in 'struct std::invoke_result<Preopterecen<
// EXPECT-CLANG: no type named 'type' in 'std::invoke_result<Preopterecen<
// POGREŠNO: visit poziva posetioca za SVAKU alternativu. Za std::string
// ne postoji lambda, pa se ne kompajlira. To je prednost nad switch-om po
// index(): zaboravljen slučaj je greška kompajliranja.
// Ispravno: dodaj [](const std::string&) { return 3; }, ili generičku
// [](const auto&) { ... } kao "sve ostalo".
#include <string>
#include <variant>
template <typename... F>
struct Preopterecen : F... {
    using F::operator()...;
};
template <typename... F>
Preopterecen(F...) -> Preopterecen<F...>;
int main() {
    std::variant<int, double, std::string> v = 5;
    return std::visit(Preopterecen{[](int) { return 1; }, [](double) { return 2; }}, v);
}
