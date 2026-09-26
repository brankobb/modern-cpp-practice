// Rešenje zadatka z2_ub_pri_kompajliranju.

#include <iostream>

// Ako fakt vraća int (nije dobro): 13! prekorači int, a to je UB koji se
// pri izvršavanju vidi samo sa UBSan-om, i to samo na liniji koja se
// izvrši. Treba ovako: dovoljno širok tip, i testovi kao static_assert --
// izvršavaju se pri kompajliranju, gde je svaki UB greška kompajlera.
constexpr long long fakt(int n) { return n <= 1 ? 1 : n * fakt(n - 1); }

static_assert(fakt(0) == 1);
static_assert(fakt(5) == 120);
static_assert(fakt(20) == 2432902008176640000LL);
#ifdef PREKORACENJE
static_assert(fakt(21) > 0);   // greška: 21! ne staje ni u long long
#endif

int main() {
    std::cout << "13! = " << fakt(13) << '\n';
    std::cout << "20! = " << fakt(20) << '\n';
}
