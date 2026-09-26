// STD: c++17
// EXPECT-GCC: no match for 'operator<<'
// EXPECT-CLANG: invalid operands to binary expression
// POGREŠNO: operator<< za ispis napisan kao member klase.
// Zašto: member operator<< bi se zvao kao v << std::cout (levi operand je
//   *this). Za std::cout << v levi operand je std::ostream, pa bi operator
//   morao biti član std::ostream-a, a tu klasu ne možeš da menjaš.
// Ispravno: slobodna funkcija (često friend):
//   friend std::ostream& operator<<(std::ostream& os, const Vec& v);
//   (main.cpp, sekcija 3 i 6).
#include <iostream>

class Vec {
public:
    explicit Vec(double x) : x_(x) {}
    std::ostream& operator<<(std::ostream& os) const { return os << x_; }

private:
    double x_;
};

int main() {
    Vec v(1.5);
    std::cout << v << "\n";
}
