// STD: c++17
// EXPECT-GCC: call to non-'constexpr' function 'int twice(int)'
// EXPECT-CLANG: constexpr variable 'q' must be initialized by a constant expression
// POGREŠNO: constexpr funkcija poziva običnu funkciju, a rezultat se traži
//   pri kompajliranju.
// Zašto: pri kompajliranju smeju da se izvrše samo constexpr funkcije.
//   quad() SAMA JE ispravna (može se zvati pri izvršavanju); greška je tek
//   kad se njen rezultat zatraži kao konstanta.
// Ispravno: constexpr int twice(int x) -- označi celu "zavisnost" kao constexpr.
int twice(int x) { return 2 * x; }

constexpr int quad(int x) { return twice(twice(x)); }

constexpr int q = quad(3);

int main() {
    return q;
}
