// EXPECT-UB: division by zero
// UB: celobrojno deljenje (i ostatak) nulom ([expr.mul]). Na x86 bez
// sanitizera program padne sa SIGFPE (test, g++ -O0), ali standard ništa
// ne garantuje.
// Ispravno: proveri divisor pre deljenja (main.cpp, sekcija 5).
#include <iostream>
int main(int argc, char**) {
    int number = 10;
    int divisor = argc - 1;    // 0
    std::cout << number / divisor << '\n';
}
