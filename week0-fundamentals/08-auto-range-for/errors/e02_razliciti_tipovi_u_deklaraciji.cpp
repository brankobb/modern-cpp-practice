// STD: c++17
// EXPECT-GCC: inconsistent deduction for 'auto': 'int' and then 'double'
// EXPECT-CLANG: deduced as 'double' in declaration of 'b'
// POGREŠNO: u jednoj deklaraciji auto mora da dobije ISTI tip za sve promenljive.
// Ispravno: auto a = 1; auto b = 2.0;
int main() {
    auto a = 1, b = 2.0;
    return a + static_cast<int>(b);
}
