// STD: c++17
// EXPECT-GCC: requires exactly one element
// EXPECT-CLANG: contains multiple expressions
// POGREŠNO: auto sa direct-list-init mora imati TAČNO jedan element (C++17, N3922).
// Ispravno: auto z = {1, 2};  (std::initializer_list<int>)
int main() {
    auto z{1, 2};
}
