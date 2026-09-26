// EXPECT-GCC: conversion from 'std::__detail::__unique_ptr_t<int>' to non-scalar type 'std::any' requested
// EXPECT-CLANG: no viable conversion from '__detail::__unique_ptr_t<int>' (aka 'unique_ptr<int>') to 'std::any'
// POGREŠNO: std::any mora da se kopira (kopija any-ja kopira sadržaj), pa
// prima samo tipove koji se kopiraju. unique_ptr je move-only.
// Ispravno: std::shared_ptr<int> (kopira se), ili tip-specifičan
// kontejner/variant umesto any.
#include <any>
#include <memory>
int main() {
    std::any a = std::make_unique<int>(5);
    return a.has_value();
}
