// STD: c++17
// EXPECT-GCC: is not literal
// EXPECT-CLANG: constexpr variable cannot have non-literal type
// POGREŠNO (u C++17): constexpr std::string.
// Zašto: u C++17 std::string nije literal tip (ima netrivijalan destruktor
//   i alocira). C++20 dozvoljava alokaciju u constexpr, ali samo PRIVREMENU:
//   memorija mora da se oslobodi pre kraja izračunavanja. Zato u C++20
//   kratak string (SSO, bez alokacije) prolazi kao constexpr promenljiva, a
//   dugačak ne (test: g++ i clang).
// Ispravno: constexpr std::string_view (main.cpp, sekcija 4), ili
//   constexpr const char*.
#include <string>

constexpr std::string greeting = "zdravo";

int main() {
    return static_cast<int>(greeting.size());
}
