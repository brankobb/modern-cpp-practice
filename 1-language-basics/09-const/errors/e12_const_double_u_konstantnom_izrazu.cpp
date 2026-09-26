// STD: c++17
// EXPECT-GCC: non-constant condition for static assertion
// EXPECT-CLANG: not an integral constant expression
// POGREŠNO (suptilno): const int sa konstantnim inicijalizatorom JESTE
// upotrebljiv u konstantnim izrazima, ali const double NIJE -- to pravilo
// važi samo za celobrojne (i enum) tipove.
// Ispravno: constexpr double d = 1.0;  -- constexpr radi za sve tipove.
int main() {
    const double d = 1.0;
    static_assert(d > 0.0);
}
