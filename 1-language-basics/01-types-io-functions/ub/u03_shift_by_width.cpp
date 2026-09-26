// EXPECT-UB: shift exponent 32 is too large for 32-bit type 'int'
// UB: pomeranje za broj bitova >= širina tipa ([expr.shift]). Na x86
// procesor uzme samo donjih 5 bitova pomeraja, pa bez sanitizera ovaj
// program ispiše 1 (test, g++ -O0) -- ali to nije garantovano, i
// kompajler sme da uradi bilo šta.
// Ispravno: proveri da je shift < širine, ili shift 64-bitni broj:
//   1ULL << 32
#include <iostream>
int main(int argc, char**) {
    int shift = 31 + argc;   // 32
    std::cout << (1 << shift) << '\n';
}
