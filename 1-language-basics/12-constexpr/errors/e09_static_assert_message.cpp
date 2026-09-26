// STD: c++17
// EXPECT-GCC: static assertion failed: Register: the type must be exactly 4 bytes
// EXPECT-CLANG: Register: the type must be exactly 4 bytes
// POGREŠNO (namerno): šablon upotrebljen sa tipom koji krši njegovo pravilo.
// Zašto: static_assert u šablonu klase proverava pravilo za SVAKI tip kojim
//   se šablon instancira, pri kompajliranju i sa porukom koju sam napišeš.
//   Tako izgleda "pogrešna upotreba biblioteke" koja se prijavi odmah, a ne
//   kao čudno ponašanje na hardveru.
// Ispravno: Register<std::uint32_t>.
#include <cstdint>

template <typename T>
struct Register {
    static_assert(sizeof(T) == 4, "Register: the type must be exactly 4 bytes");
    T value;
};

int main() {
    Register<std::uint16_t> r{};
    return r.value;
}
