// EXPECT-UB: signed integer overflow
// POGREŠNO: constexpr funkcija pozvana pri izvršavanju sa vrednostima koje
//   prekorače int.
// Zašto: constexpr ne menja pravila jezika pri izvršavanju: signed overflow
//   je UB kao i inače. Pri kompajliranju bi isti poziv bio greška (errors/e01);
//   ovde se argument zna tek u runtime-u (argc), pa greške nema.
// Ispravno: proveri opseg pre sabiranja, ili širi tip.
#include <climits>
#include <cstdio>

constexpr int add(int a, int b) { return a + b; }

int main(int argc, char**) {
    int big = INT_MAX - 1 + argc;
    std::printf("%d\n", add(big, 1));
}
