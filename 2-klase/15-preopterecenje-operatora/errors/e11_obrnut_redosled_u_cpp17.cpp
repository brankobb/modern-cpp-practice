// STD: c++17
// EXPECT-GCC: no match for 'operator==' (operand types are 'int' and 'Meters')
// EXPECT-CLANG: invalid operands to binary expression ('int' and 'Meters')
// POGREŠNO (u C++17): samo member operator==(int), a poziv 5 == m.
// Zašto: member se poziva na levom operandu, a levo je int. U C++17 je
//   trebao poseban operator==(int, const Meters&), i još dva za !=. C++20
//   prepisuje 5 == m u m == 5 i m != 5 u !(m == 5), pa je u C++20 ovaj
//   kod ispravan (main_cpp20.cpp, sekcija 3).
// Ispravno u C++17: sve četiri kombinacije kao slobodne (friend) funkcije.
struct Meters {
    int value;
    bool operator==(int other) const { return value == other; }
};

int main() {
    Meters m{5};
    return 5 == m;
}
