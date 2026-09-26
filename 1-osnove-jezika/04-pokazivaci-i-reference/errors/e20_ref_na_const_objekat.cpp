// STD: c++17
// EXPECT-GCC: discards qualifiers
// EXPECT-CLANG: drops 'const' qualifier
// POGREŠNO: non-const referenca na const objekat bi dozvolila izmenu.
// Ispravno: const int& r = c;
int main() {
    const int c = 1;
    int& r = c;
    return r;
}
