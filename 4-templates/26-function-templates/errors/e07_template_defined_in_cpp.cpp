// LINK: support/max_of.cpp
// EXPECT-GCC: undefined reference to `int maxOf<int>(int, int)'
// EXPECT-CLANG: undefined reference to `int maxOf<int>(int, int)'
// POGREŠNO (greška LINKERA): šablon je u header-u samo DEKLARISAN, a
// definisan u max_of.cpp. Ovaj fajl vidi deklaraciju, pa se kompajlira i
// očekuje funkciju maxOf<int> negde drugde. max_of.cpp vidi definiciju, ali
// maxOf<int> nigde ne koristi, pa je ne instancira. Rezultat: nikome nije
// napravljena, i linker je ne nalazi.
// Ispravno: definicija šablona ide u HEADER (šabloni su implicitno "inline"
// po ODR-u, lekcija 08). Ili, za unapred poznat skup tipova, eksplicitna
// instancijacija u max_of.cpp:   template int maxOf<int>(int, int);
#include "support/max_of.h"
int main() { return maxOf(1, 2) == 2 ? 0 : 1; }
