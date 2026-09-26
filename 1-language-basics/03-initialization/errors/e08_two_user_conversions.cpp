// STD: c++17
// EXPECT-GCC: to non-scalar type 'Name'
// EXPECT-CLANG: no viable conversion from 'const char
// POGREŠNO: copy-init dozvoljava najviše JEDNU korisničku konverziju.
// "John" -> std::string (1. konverzija) -> Name (2. konverzija) = previše.
// Ispravno: Name n("John");  -- direct-init, samo jedna konverzija (u argument)
#include <string>
struct Name {
    Name(std::string s) : v(s) {}
    std::string v;
};
int main() {
    Name n = "John";
    (void)n;
}
