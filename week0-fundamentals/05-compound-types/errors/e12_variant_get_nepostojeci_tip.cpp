// STD: c++17
// EXPECT-GCC: T must occur exactly once in alternatives
// EXPECT-CLANG: T must occur exactly once in alternatives
// POGREŠNO: std::get<T> na variant-u proverava PRI KOMPAJLIRANJU da li je T
// jedna od alternativa. double nije.
// (Pogrešna alternativa koja POSTOJI je runtime greška: std::bad_variant_access.)
#include <string>
#include <variant>
int main() {
    std::variant<int, std::string> v = 1;
    return static_cast<int>(std::get<double>(v));
}
