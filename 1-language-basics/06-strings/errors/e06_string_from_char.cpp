// STD: c++17
// EXPECT-GCC: conversion from 'char' to non-scalar type 'std::string'
// EXPECT-CLANG: no viable conversion from 'char' to 'std::string'
// POGREŠNO: std::string s = 'a';
// Zašto: std::string nema konstruktor iz jednog char-a. Zbunjujuće, jer
//   DODELA postoji: s = 'a'; (i s = 65; tiho postane "A", main.cpp
//   sekcija 4).
// Ispravno: std::string s(1, 'a'); ili std::string s{'a'}; (initializer_list)
//   ili std::string s = "a";
#include <string>

int main() {
    std::string s = 'a';
    return static_cast<int>(s.size());
}
