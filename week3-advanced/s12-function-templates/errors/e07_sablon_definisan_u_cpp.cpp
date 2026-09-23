// LINK: support/maks.cpp
// EXPECT-GCC: undefined reference to `int maks<int>(int, int)'
// EXPECT-CLANG: undefined reference to `int maks<int>(int, int)'
// POGREŠNO (greška LINKERA): šablon je u header-u samo DEKLARISAN, a
// definisan u maks.cpp. Ovaj fajl vidi deklaraciju, pa se kompajlira i
// očekuje funkciju maks<int> negde drugde. maks.cpp vidi definiciju, ali
// maks<int> nigde ne koristi, pa je ne instancira. Rezultat: nikome nije
// napravljena, i linker je ne nalazi.
// Ispravno: definicija šablona ide u HEADER (šabloni su implicitno "inline"
// po ODR-u, lekcija 06). Ili, za unapred poznat skup tipova, eksplicitna
// instancijacija u maks.cpp:   template int maks<int>(int, int);
#include "support/maks.h"
int main() { return maks(1, 2) == 2 ? 0 : 1; }
