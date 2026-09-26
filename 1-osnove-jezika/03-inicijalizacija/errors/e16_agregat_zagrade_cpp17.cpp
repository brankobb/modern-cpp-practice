// STD: c++17
// EXPECT-GCC: no matching function for call to 'Point::Point(int, int)'
// EXPECT-CLANG: no matching constructor for initialization of 'Point'
// POGREŠNO u C++17: agregat se ne može inicijalizovati sa ().
// U C++20 ovo RADI (P0960, "parenthesized aggregate initialization") -- zato
// std::make_unique<Point>(1, 2) radi tek od C++20.
struct Point {
    int x;
    int y;
};
int main() {
    Point p(1, 2);
    (void)p;
}
