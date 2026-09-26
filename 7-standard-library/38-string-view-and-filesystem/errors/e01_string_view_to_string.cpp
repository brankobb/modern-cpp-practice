// EXPECT-GCC: conversion from 'std::string_view' {aka 'std::basic_string_view<char>'} to non-scalar type 'std::string'
// EXPECT-CLANG: no viable conversion from 'std::string_view' (aka 'basic_string_view<char>') to 'std::string'
// POGREŠNO: string -> string_view je implicitno (jeftino: pokazivač +
// dužina), ali string_view -> string NIJE: to je alokacija i kopija, pa
// standard traži da se napiše.
// Ispravno: std::string s(sv);  ili  std::string s{sv};
#include <string>
#include <string_view>
int main() {
    std::string_view sv = "temp";
    std::string s = sv;
    return static_cast<int>(s.size());
}
