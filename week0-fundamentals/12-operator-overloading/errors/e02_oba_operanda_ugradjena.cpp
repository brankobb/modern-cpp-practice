// STD: c++17
// EXPECT-GCC: must have an argument of class or enumerated type
// EXPECT-CLANG: overloaded 'operator+' must have at least one parameter of class or enumeration type
// POGREŠNO: operator+ za dva int-a.
// Zašto: bar jedan operand mora biti klasa ili enum ([over.oper]). Inače bi
//   neko mogao da promeni značenje 1 + 2 za ceo program.
// Ispravno: operator za SVOJ tip (Money operator+(Money, const Money&)),
//   ili obična funkcija za ugrađene tipove.
int operator+(int a, int b) { return a - b; }

int main() {
    return 1 + 2;
}
