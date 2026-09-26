// STD: c++17
// EXPECT-GCC: declaration of 'auto x' has no initializer
// EXPECT-CLANG: requires an initializer
// POGREŠNO: auto dedukuje tip IZ inicijalizatora -- bez njega nema odakle.
// To je i prednost (EMC Item 5): auto promenljiva ne može ostati neinicijalizovana.
int main() {
    auto x;
    return x;
}
