// STD: c++17
// EXPECT-GCC: std::vector must have a non-const, non-volatile value_type
// EXPECT-CLANG: std::vector must have a non-const, non-volatile value_type
// POGREŠNO: std::vector<const int>.
// Zašto: vektor mora da pomera i dodeljuje elemente (realokacija, insert,
//   erase), a const element ne može da se dodeli. Standard to zabranjuje za
//   allocator-aware kontejnere; libstdc++ ima static_assert sa jasnom porukom.
// Ispravno: const std::vector<int> (ceo vektor je nepromenljiv), ili
//   std::vector<int> iza const& u interfejsu.
#include <vector>

int main() {
    std::vector<const int> v{1, 2};
    return v[0];
}
