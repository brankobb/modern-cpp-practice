// STD: c++17
// EXPECT-GCC: has invalid argument list
// EXPECT-CLANG: invalid literal operator parameter type 'double', did you mean 'long double'?
// POGREŠNO: operator""_km(double).
// Zašto: literal operator sme da ima samo tačno propisane parametre
//   ([over.literal]): za brojeve sa decimalama long double, za cele brojeve
//   unsigned long long, za stringove (const char*, std::size_t), ili
//   const char* za "sirov" oblik. double nije među njima.
// Ispravno: operator""_km(long double v), i posebno
//   operator""_km(unsigned long long v) ako treba i 3_km (main.cpp, sekcija 8).
struct Meters {
    double value;
};

constexpr Meters operator""_km(double v) { return Meters{v * 1000}; }

int main() {
    return static_cast<int>((1.5_km).value);
}
