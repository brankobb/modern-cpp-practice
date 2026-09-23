// EXPECT-GCC: 'x' is not a constant expression
// EXPECT-CLANG: constexpr if condition is not a constant expression
// POGREŠNO: uslov if constexpr se računa pri KOMPAJLIRANJU; parametar
// funkcije je poznat tek pri pokretanju.
// Ispravno: običan if (if (x > granica) ...). if constexpr je za uslove
// od tipova i konstanti: std::is_integral_v<T>, sizeof...(args), N > 0.
int ogranici(int x, int granica) {
    if constexpr (x > granica) return granica;
    return x;
}
int main() { return ogranici(5, 3); }
