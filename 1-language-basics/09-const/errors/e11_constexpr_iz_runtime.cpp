// STD: c++17
// EXPECT-GCC: call to non-'constexpr' function
// EXPECT-CLANG: must be initialized by a constant expression
// POGREŠNO: constexpr promenljiva MORA da se izračuna pri kompajliranju.
// Razlika u odnosu na const: const prihvata runtime vrednost, constexpr ne.
#include <cstdlib>
int main() {
    constexpr int n = std::rand();
    return n;
}
