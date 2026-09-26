// STD: c++17
// EXPECT-GCC: narrowing conversion
// EXPECT-CLANG: cannot be narrowed
// POGREŠNO: narrowing zabrana važi i za aggregate initialization.
struct Point {
    int x;
    int y;
};
int main() {
    Point p{1.5, 2};
    (void)p;
}
