// STD: c++17
// EXPECT-GCC: is not usable in a constant expression
// EXPECT-CLANG: is not a constant expression
// POGREŠNO: const NE znači "poznato pri kompajliranju". n se računa u
// runtime-u, pa ne može biti veličina std::array-a (template argument).
// Ispravno: constexpr int n = 3;  ili std::vector<int> v(n);
#include <array>
#include <cstdlib>
int main() {
    const int n = std::rand() % 10 + 1;
    std::array<int, n> a{};
    return a[0];
}
