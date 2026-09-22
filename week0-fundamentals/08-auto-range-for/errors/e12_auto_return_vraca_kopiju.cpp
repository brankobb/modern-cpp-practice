// STD: c++17
// EXPECT-GCC: lvalue required as left operand of assignment
// EXPECT-CLANG: expression is not assignable
// POGREŠNO (EMC Item 3): auto kao povratni tip odbacuje referencu -- vraća
// KOPIJU (rvalue), pa dodela rezultatu nije moguća.
// Ispravno: decltype(auto) authAndAccess(...)  -- zadržava int& (main.cpp, sekcija 4)
#include <vector>
template <typename Container, typename Index>
auto accessCopy(Container& c, Index i) {
    return c[i];
}
int main() {
    std::vector<int> v{1, 2, 3};
    accessCopy(v, 0) = 10;
}
