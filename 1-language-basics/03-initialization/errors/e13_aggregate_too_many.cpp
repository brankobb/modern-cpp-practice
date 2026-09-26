// STD: c++17
// EXPECT-GCC: too many initializers
// EXPECT-CLANG: excess elements in struct initializer
// POGREŠNO: više inicijalizatora nego članova agregata.
struct Point {
    int x;
    int y;
};
int main() {
    Point p{1, 2, 3};
    (void)p;
}
