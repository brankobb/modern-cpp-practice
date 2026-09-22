// STD: c++17
// EXPECT-GCC: no matching function for call to 'std::unique_ptr<_IO_FILE, int (*)(_IO_FILE*)>::unique_ptr(FILE*)'
// EXPECT-CLANG: no matching constructor for initialization of 'std::unique_ptr<std::FILE, decltype(&std::fclose)>'
// POGREŠNO: unique_ptr sa POKAZIVAČEM NA FUNKCIJU kao deleterom, a sama
//   funkcija nije prosleđena.
// Zašto: tip deletera je "int (*)(FILE*)", a vrednost je nullptr dok je ne
//   proslediš. unique_ptr zato ne dozvoljava konstruktor bez deletera; inače
//   bi destruktor pozvao null pokazivač na funkciju.
// Ispravno: std::unique_ptr<std::FILE, decltype(&std::fclose)> f(std::tmpfile(), &std::fclose);
//   Još bolje: funkcijski objekat kao deleter (struct FileCloser, main.cpp
//   sekcija 6). Ne mora da se prosleđuje, a unique_ptr ostaje veličine
//   jednog pokazivača (test: 8 bajtova, a sa pokazivačem na funkciju 16).
#include <cstdio>
#include <memory>

int main() {
    std::unique_ptr<std::FILE, decltype(&std::fclose)> f(std::tmpfile());
    return f != nullptr;
}
