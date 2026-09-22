// STD: c++17
// EXPECT-GCC: must have either one or two arguments
// EXPECT-CLANG: overloaded 'operator+' must be a unary or binary operator (has 3 parameters)
// POGREŠNO: operator+ sa tri parametra.
// Zašto: preopterećen operator zadržava broj operanada ugrađenog: + je
//   unarni (+a) ili binarni (a + b). Kao member, levi operand je *this, pa
//   member operator+ ima nula ili jedan parametar. Izuzetak je operator(),
//   koji sme da ima proizvoljan broj parametara.
// Ispravno: a + b + c radi samo od sebe kao (a + b) + c.
struct Vec {
    double x;
};

Vec operator+(const Vec& a, const Vec& b, const Vec& c) { return Vec{a.x + b.x + c.x}; }

int main() {}
