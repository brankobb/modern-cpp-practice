// STD: c++17
// EXPECT-GCC: return type specified for 'operator int'
// EXPECT-CLANG: conversion function cannot have a return type
// POGREŠNO: operator konverzije sa napisanim povratnim tipom.
// Zašto: kod operator int() ime je već tip u koji se konvertuje, pa se
//   povratni tip ne piše ([class.conv.fct]). Ne prima ni parametre.
// Ispravno: explicit operator int() const { ... }  (explicit je bolji
//   izbor, C.164).
struct Meters {
    double v;
    int operator int() const { return static_cast<int>(v); }
};

int main() {}
