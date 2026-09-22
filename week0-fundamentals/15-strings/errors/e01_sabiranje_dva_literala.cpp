// STD: c++17
// EXPECT-GCC: invalid operands of types 'const char [9]' and 'const char [6]' to binary 'operator+'
// EXPECT-CLANG: invalid operands to binary expression ('const char[9]' and 'const char[6]')
// POGREŠNO: "tekst" + "tekst".
// Zašto: literali su nizovi char-ova, ne std::string. Oba se raspadnu u
//   const char*, a dva pokazivača ne mogu da se saberu. operator+ za
//   stringove postoji samo kad je BAR jedan operand std::string.
// Ispravno: "Zdravo, " "svete" (spoji ih kompajler), std::string("Zdravo, ")
//   + "svete", ili "Zdravo, "s + "svete" (main.cpp, sekcija 8).
#include <string>

int main() {
    std::string s = "Zdravo, " + "svete";
    return static_cast<int>(s.size());
}
