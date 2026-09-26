// EXPECT-GCC: 'x' is not a constant expression
// EXPECT-CLANG: constexpr if condition is not a constant expression
// POGREŠNO: uslov if constexpr se računa pri KOMPAJLIRANJU; parametar
// funkcije je poznat tek pri pokretanju.
// Ispravno: običan if (if (x > granica) ...). if constexpr je za uslove
// od tipova i konstanti: std::is_integral_v<T>, sizeof...(args), N > 0.
int clampTo(int x, int limit) {
    if constexpr (x > limit) return limit;
    return x;
}
int main() { return clampTo(5, 3); }
