// STD: c++17
// EXPECT-GCC: 'y' declared as reference but not initialized
// EXPECT-CLANG: declaration of reference variable 'y' requires an initializer
// POGREŠNO -- ali poučno (EMC Item 3): decltype(x) je int, a decltype((x))
// je int& -- (x) je izraz (lvalue), ne ime. Referenca mora da se inicijalizuje,
// pa ova linija ne prolazi i time otkriva pravi tip.
int main() {
    int x = 0;
    decltype((x)) y;
    return x + y;
}
