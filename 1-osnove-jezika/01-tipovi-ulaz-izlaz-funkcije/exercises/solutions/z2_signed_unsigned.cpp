// Rešenje zadatka z2_signed_unsigned.

#include <iostream>
#include <utility>

// Ako porediš int sa unsigned direktno (nije dobro): int se konvertuje u
// unsigned, pa svaki negativan broj postane ogroman i "nije manji" ni od
// čega.
// Treba ovako: negativan slučaj reši pre konverzije, pa poredi dva unsigned.
bool trebaGrejanje(int temp, unsigned prag) {
    if (temp < 0) return true;
    return static_cast<unsigned>(temp) < prag;
}

// Možeš i ovako (C++20): std::cmp_less poredi matematički tačno, bez
// obzira na signedness.
#if __cplusplus >= 202002L
static_assert(std::cmp_less(-5, 18u));
#endif

int main() {
    const unsigned prag = 18;
    for (int temp : {25, 5, -5})
        std::cout << "temp " << temp << ": grejač " << (trebaGrejanje(temp, prag) ? "uključen" : "isključen")
                  << '\n';
}
