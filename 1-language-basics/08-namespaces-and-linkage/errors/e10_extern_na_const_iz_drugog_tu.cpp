// STD: c++17
// LINK: support/const_limit.cpp
// EXPECT-GCC: undefined reference to `limit'
// EXPECT-CLANG: undefined reference to `limit'
// POGREŠNO: "extern const int limit;" ovde, a u drugom .cpp "const int limit = 10;".
// Zašto: const promenljiva na nivou namespace-a ima INTERNAL linkage
//   ([basic.link]), pa je limit u const_limit.cpp nevidljiv linkeru. Ovde je
//   deklarisan kao extern i traži se spolja. Razlika od C-a: u C-u const
//   globalna promenljiva ima external linkage.
// Ispravno: "extern const int limit = 10;" u jednom .cpp (extern menja
//   linkage), ili "inline constexpr int limit = 10;" u header-u.
extern const int limit;

int main() {
    return limit;
}
