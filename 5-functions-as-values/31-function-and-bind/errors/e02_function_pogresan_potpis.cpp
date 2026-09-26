// EXPECT-GCC: conversion from 'main()::<lambda(const std::string&)>' to non-scalar type 'std::function<int(int)>' requested
// EXPECT-CLANG: no viable conversion from
// POGREŠNO: std::function<int(int)> prima samo ono što se može pozvati sa
// jednim int-om i vratiti nešto što se konvertuje u int. Lambda traži
// std::string, a int se ne konvertuje u std::string.
// Ispravno: potpis std::function-a mora da odgovara upotrebi --
// std::function<int(const std::string&)> -- ili lambda koja prima int.
#include <functional>
#include <string>
int main() {
    std::function<int(int)> f = [](const std::string& s) { return static_cast<int>(s.size()); };
    return f(1);
}
