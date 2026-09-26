// STD: c++17
// EXPECT-GCC: conversion from 'int*' to non-scalar type 'std::unique_ptr<int>' requested
// EXPECT-CLANG: no viable conversion from 'int *' to 'std::unique_ptr<int>'
// POGREŠNO: std::unique_ptr<int> p = new int(5);
// Zašto: konstruktor unique_ptr(T*) je explicit, a "= izraz" je copy
//   inicijalizacija, koja explicit konstruktore ne koristi (lekcija 03).
//   Namerno je tako: unique_ptr preuzima vlasništvo, pa to ne sme da se
//   desi tiho, npr. pri prosleđivanju sirovog pokazivača funkciji.
// Ispravno: auto p = std::make_unique<int>(5); (EMC Item 21), ili
//   std::unique_ptr<int> p(new int(5));
#include <memory>

int main() {
    std::unique_ptr<int> p = new int(5);
}
