// EXPECT-GCC: has no member named 'size'
// EXPECT-CLANG: no member named 'size' in 'std::optional<std::basic_string<char>>'
// POGREŠNO: optional nije string -- on SADRŽI string. Članovi sadržaja
// se dohvataju kao kroz pokazivač: o->size() ili (*o).size() (oba bez
// provere), ili o.value().size() (sa proverom).
// (clang predloži: did you mean to use '->' instead of '.'?)
// Ispravno: if (o) return static_cast<int>(o->size());
#include <optional>
#include <string>
int main() {
    std::optional<std::string> o = "abc";
    return static_cast<int>(o.size());
}
