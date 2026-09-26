// STD: c++17
// EXPECT-GCC: declared as reference but not initialized
// EXPECT-CLANG: requires an initializer
// POGREŠNO: referenca MORA da se veže pri deklaraciji -- nema "prazne" reference.
int main() {
    int& r;
    return r;
}
