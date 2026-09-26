// FLAGS: -fsanitize=float-cast-overflow
// EXPECT-UB: is outside the range of representable values of type 'int'
// POGREŠNO: static_cast<int> vrednosti 1e10, koja ne staje u int.
// Zašto: double -> int odseca decimale, ali ako CEO deo ne staje u int,
//   ponašanje je nedefinisano ([conv.fpint]). Za razliku od int -> unsigned
//   (definisano, modulo) ili int -> short (od C++20 definisano, modulo).
//   Test: ispisuje -2147483648, a na drugoj platformi može biti bilo šta.
//   g++ -fsanitize=undefined ovo NE uključuje; treba -fsanitize=float-cast-overflow.
// Ispravno: proveri opseg pre konverzije (std::numeric_limits<int>), ili
//   proverena konverzija narrow<int>(...) (main.cpp, sekcija 8).
#include <cstdio>

int main() {
    double big = 1e10;
    int n = static_cast<int>(big);
    std::printf("%d\n", n);
}
