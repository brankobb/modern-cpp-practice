// STD: c++17
// EXPECT-GCC: result type must be constructible from input type
// EXPECT-CLANG: result type must be constructible from input type
// POGREŠNO: std::vector<std::unique_ptr<int>> v{make_unique(...), ...}.
// Zašto: initializer_list pokazuje na niz CONST elemenata. Vektor mora da ih
//   KOPIRA (iz const objekta se ne može pomerati), a unique_ptr nema kopiju.
//   Poruka dolazi iz dubine libstdc++ (static_assert), a ne sa ove linije.
// Ispravno: prazan vektor pa push_back / emplace_back (main.cpp, sekcija 6);
//   ili reserve(n) pa emplace_back u petlji.
#include <memory>
#include <vector>

int main() {
    std::vector<std::unique_ptr<int>> v{std::make_unique<int>(1), std::make_unique<int>(2)};
    return static_cast<int>(v.size());
}
