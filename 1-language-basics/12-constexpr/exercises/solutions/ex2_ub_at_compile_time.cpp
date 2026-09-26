// Rešenje zadatka ex2_ub_at_compile_time.

#include <iostream>

// Ako fact vraća int (nije dobro): 13! prekorači int, a to je UB koji se
// pri izvršavanju vidi samo sa UBSan-om, i to samo na liniji koja se
// izvrši. Treba ovako: dovoljno širok tip, i testovi kao static_assert --
// izvršavaju se pri kompajliranju, gde je svaki UB greška kompajlera.
constexpr long long fact(int n) { return n <= 1 ? 1 : n * fact(n - 1); }

static_assert(fact(0) == 1);
static_assert(fact(5) == 120);
static_assert(fact(20) == 2432902008176640000LL);
#ifdef FACT_21
static_assert(fact(21) > 0);   // greška: 21! ne staje ni u long long
#endif

int main() {
    std::cout << "13! = " << fact(13) << '\n';
    std::cout << "20! = " << fact(20) << '\n';
}
