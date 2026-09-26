// STD: c++17
// EXPECT-GCC: cannot convert 'Color' to 'int'
// EXPECT-CLANG: of type 'int' with an rvalue of type 'Color'
// POGREŠNO: enum class se NE konvertuje implicitno u int -- to je i poenta.
// Ispravno: int n = static_cast<int>(Color::Red);
enum class Color { Red, Green };
int main() {
    int n = Color::Red;
    return n;
}
