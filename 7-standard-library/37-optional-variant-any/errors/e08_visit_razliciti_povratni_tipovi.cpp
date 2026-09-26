// EXPECT-GCC: std::visit requires the visitor to have the same return type for all alternatives of a variant
// EXPECT-CLANG: std::visit requires the visitor to have the same return type for all alternatives of a variant
// POGREŠNO: visit vraća JEDAN tip, pa sve grane posetioca moraju da
// vrate isti. Ovde prva vraća const char* ("ok"), a druga std::string.
// (Nađeno pri pisanju rešenja zadatka ex1.)
// Ispravno: isti povratni tip svuda -- [](int) -> std::string { return "ok"; }
#include <string>
#include <variant>
template <typename... F>
struct Preopterecen : F... {
    using F::operator()...;
};
template <typename... F>
Preopterecen(F...) -> Preopterecen<F...>;
int main() {
    std::variant<int, std::string> v = 5;
    auto r = std::visit(Preopterecen{[](int) { return "ok"; }, [](const std::string& s) { return s + "!"; }}, v);
    return static_cast<int>(r.size());
}
