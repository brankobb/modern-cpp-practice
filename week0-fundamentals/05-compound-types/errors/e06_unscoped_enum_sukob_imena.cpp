// STD: c++17
// EXPECT-GCC: 'Red' conflicts with a previous declaration
// EXPECT-CLANG: redefinition of enumerator 'Red'
// POGREŠNO (EMC Item 10): imena iz obične enum "cure" u okolni scope, pa se
// dve enum sa istim imenom elementa sudaraju.
// Ispravno: enum class Color { Red }; enum class Alert { Red };  -> Color::Red, Alert::Red
enum Color { Red, Green };
enum Alert { Red, Yellow };
int main() {}
