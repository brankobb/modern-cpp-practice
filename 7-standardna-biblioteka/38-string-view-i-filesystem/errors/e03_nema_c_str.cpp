// EXPECT-GCC: has no member named 'c_str'
// EXPECT-CLANG: no member named 'c_str' in 'std::basic_string_view<char>'
// POGREŠNO: c_str() obećava '\0' na kraju. string_view to ne može da
// obeća -- može biti deo većeg stringa (substr). Ima data(), ali to NIJE
// C string (main.cpp, sekcija 3; zadatak z3).
// Ispravno: std::string(sv).c_str() -- kopija sa '\0'; ili funkcija koja
// prima dužinu: std::fwrite(sv.data(), 1, sv.size(), stdout).
#include <cstdio>
#include <string_view>
int main() {
    std::string_view sv = "temp";
    std::puts(sv.c_str());
}
