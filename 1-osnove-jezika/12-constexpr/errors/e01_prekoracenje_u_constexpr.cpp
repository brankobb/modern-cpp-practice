// STD: c++17
// EXPECT-GCC: overflow in constant expression
// EXPECT-CLANG: constexpr variable 'total' must be initialized by a constant expression
// POGREŠNO: prekoračenje int-a u izračunavanju koje mora biti konstantno.
// Zašto: kompajler koji računa konstantni izraz MORA da odbije UB
//   ([expr.const]): overflow, pristup van niza, deljenje nulom, čitanje
//   neinicijalizovane vrednosti. Ista funkcija pozvana pri izvršavanju tiho
//   ima UB (ub/u01). Zato je constexpr + static_assert besplatan "sanitizer"
//   za logiku koja može da se proveri unapred.
// Ispravno: int64_t za rezultat, ili provera pre sabiranja
//   (b > 0 && a > INT_MAX - b).
#include <climits>

constexpr int add(int a, int b) { return a + b; }

constexpr int total = add(INT_MAX, 1);

int main() {
    return total;
}
