// STD: c++17
// EXPECT-GCC: only available with '-std=c++20'
// EXPECT-CLANG: defaulted comparison operators are a C++20 extension
// POGREŠNO: "bool operator==(const Point&) const = default;" u C++17.
// Zašto: kompajlerom generisano poređenje (= default za ==, !=, <, <=>)
//   postoji od C++20 (P0515, P1185). U C++17 se piše ručno.
// Ispravno: u C++17 ručno (return x == o.x && y == o.y;), ili
//   std::tie(x, y) == std::tie(o.x, o.y). U C++20 ova linija je ispravna
//   (main_cpp20.cpp).
struct Point {
    int x;
    int y;
    bool operator==(const Point&) const = default;
};

int main() {
    Point a{1, 2};
    Point b{1, 2};
    return a == b;
}
