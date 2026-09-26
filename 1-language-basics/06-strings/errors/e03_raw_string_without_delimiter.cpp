// STD: c++17
// EXPECT-GCC: missing terminating " character
// EXPECT-CLANG: expected ';' at end of declaration
// POGREŠNO: raw string čiji tekst sadrži )" a nema sopstveni graničnik.
// Zašto: R"( ... )" se završava na PRVOM )" u tekstu. Ovde je to posle
//   "the end is ", a ostatak ( here)"; ) više nije deo stringa. Poruke oba
//   kompajlera pokazuju na posledicu, a ne na uzrok.
// Ispravno: sopstveni graničnik koji se ne pojavljuje u tekstu:
//   R"x(the end is )" here)x"  (main.cpp, sekcija 2).
#include <cstdio>

int main() {
    const char* s = R"(the end is )" here)";
    std::puts(s);
}
