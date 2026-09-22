// STD: c++17
// EXPECT-GCC: ambiguating new declaration of 'double f()'
// EXPECT-CLANG: functions that differ only in their return type cannot be overloaded
// POGREŠNO: overload se razlikuje po PARAMETRIMA, ne po povratnom tipu --
// na mestu poziva f() kompajler ne bi znao koju da izabere.
int f();
double f();
int main() {}
