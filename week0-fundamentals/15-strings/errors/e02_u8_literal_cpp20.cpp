// STD: c++20
// EXPECT-GCC: invalid conversion from 'const char8_t*' to 'const char*'
// EXPECT-CLANG: cannot initialize a variable of type 'const char *' with an lvalue of type 'const char8_t[7]'
// POGREŠNO (u C++20): u8"..." dodeljen const char*.
// Zašto: C++20 (P0482) uvodi char8_t, i u8 literal je od tada niz char8_t,
//   a ne char. Isti kod se u C++17 kompajlira. Ovo je jedna od retkih
//   promena koje kvare postojeći kod pri prelasku na C++20.
// Ispravno: "čaša" bez prefiksa (fajl sačuvan kao UTF-8, pa je i literal
//   UTF-8), ili const char8_t* / std::u8string ako namerno radiš sa char8_t.
int main() {
    const char* s = u8"čaša";
    return s[0];
}
