// EXPECT-UB: division by zero
// UB: celobrojno deljenje (i ostatak) nulom ([expr.mul]). Na x86 bez
// sanitizera program padne sa SIGFPE (test, g++ -O0), ali standard ništa
// ne garantuje.
// Ispravno: proveri delilac pre deljenja (main.cpp, sekcija 5).
#include <iostream>
int main(int argc, char**) {
    int broj = 10;
    int delilac = argc - 1;    // 0
    std::cout << broj / delilac << '\n';
}
