// STD: c++17
// EXPECT-GCC: 'Red' was not declared in this scope
// EXPECT-CLANG: use of undeclared identifier 'Red'
// POGREŠNO: elementi enum class žive u njenom scope-u -- piše se Color::Red.
// (C++20: "using enum Color;" ih uvozi u trenutni scope.)
enum class Color { Red, Green };
int main() {
    Color c = Red;
    return static_cast<int>(c);
}
