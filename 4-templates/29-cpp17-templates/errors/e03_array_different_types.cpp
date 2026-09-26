// EXPECT-GCC: class template argument deduction failed
// EXPECT-CLANG: no viable constructor or deduction guide for deduction of template arguments of 'array'
// POGREŠNO: vodič za std::array traži da SVI elementi budu istog tipa
// (clang u napomeni kaže: requirement 'is_same_v<int, double>'). Nema
// tihog izbora "zajedničkog" tipa.
// Ispravno: navedi tip -- std::array<double, 3> a{1, 2.5, 3};
// ili svi literali istog tipa: std::array a{1.0, 2.5, 3.0};
#include <array>
int main() {
    std::array a{1, 2.5, 3};
    return static_cast<int>(a[0]);
}
