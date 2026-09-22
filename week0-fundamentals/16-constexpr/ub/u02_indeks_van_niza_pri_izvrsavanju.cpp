// EXPECT-UB: index 3 out of bounds for type 'int \[3\]'
// POGREŠNO: constexpr funkcija pozvana pri izvršavanju sa indeksom van niza.
// Zašto: pri kompajliranju at(3) je greška (errors/e02), pri izvršavanju
//   tihi UB. UBSan (-fsanitize=bounds) ga hvata jer je veličina niza poznata.
// Ispravno: provera 0 <= i < 3 pre pristupa.
#include <cstdio>

constexpr int table[3] = {10, 20, 30};

constexpr int at(int i) { return table[i]; }

int main(int argc, char**) {
    std::printf("%d\n", at(argc + 2));
}
