// STD: c++17
// EXPECT-GCC: to non-scalar type 'array<[...],4>'
// EXPECT-CLANG: no viable conversion from 'array<[...], 3>' to 'array<[...], 4>'
// POGREŠNO: veličina je deo TIPA std::array-a -- array<int, 3> i array<int, 4>
// su različiti tipovi, bez konverzije.
#include <array>
int main() {
    std::array<int, 3> a{};
    std::array<int, 4> b = a;
    return b[0];
}
