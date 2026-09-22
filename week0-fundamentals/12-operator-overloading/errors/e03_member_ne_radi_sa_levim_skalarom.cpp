// STD: c++17
// EXPECT-GCC: no match for 'operator*' (operand types are 'double' and 'Vec')
// EXPECT-CLANG: invalid operands to binary expression ('double' and 'Vec')
// POGREŠNO: operator* samo kao member, pa korišćen kao 2.0 * v.
// Zašto: member operator se poziva na LEVOM operandu (v * 2.0 je
//   v.operator*(2.0)). Za 2.0 * v bi trebalo double::operator*(Vec), a
//   double nema članove. Konverzije se primenjuju samo na argumente, a
//   levi operand member operatora nije argument (EC++ Item 24).
// Ispravno: slobodna funkcija Vec operator*(double s, const Vec& v), kao u
//   main.cpp, sekcija 2 (Rational).
class Vec {
public:
    Vec(double x, double y) : x_(x), y_(y) {}
    Vec operator*(double s) const { return Vec(x_ * s, y_ * s); }
    double x() const { return x_; }

private:
    double x_;
    double y_;
};

int main() {
    Vec v(1, 2);
    Vec ok = v * 2.0;
    Vec bad = 2.0 * v;
    return static_cast<int>(ok.x() + bad.x());
}
