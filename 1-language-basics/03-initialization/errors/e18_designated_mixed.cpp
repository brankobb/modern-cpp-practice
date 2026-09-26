// STD: c++20
// EXPECT-GCC: either all initializer clauses should be designated
// EXPECT-CLANG: mixture of designated and non-designated
// POGREŠNO: ne smeš mešati designated i pozicione inicijalizatore.
// clang bez -pedantic-errors ovo prihvata kao C99 ekstenziju (samo warning).
struct Point {
    int x;
    int y;
};
int main() {
    Point p{.x = 1, 2};
    (void)p;
}
