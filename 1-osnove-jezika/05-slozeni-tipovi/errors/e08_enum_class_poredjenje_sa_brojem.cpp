// STD: c++17
// EXPECT-GCC: no match for 'operator<' (operand types are 'Color' and 'int')
// EXPECT-CLANG: invalid operands to binary expression ('Color' and 'int')
// POGREŠNO: enum class se ne poredi sa brojem -- kod obične enum ovo tiho
// prolazi (i Color < 14.5 bi se kompajliralo, EMC Item 10).
enum class Color { Red, Green };
int main() {
    Color c = Color::Green;
    return c < 1;
}
