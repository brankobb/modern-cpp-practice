// EXPECT-UB: division of -2147483648 by -1 cannot be represented in type 'int'
// UB: INT_MIN / -1 = 2147483648, a to ne staje u int ([expr.mul]: ako
// rezultat nije predstavljiv, ponašanje je nedefinisano). Isto važi i za
// INT_MIN % -1. Na x86 bez sanitizera program padne sa SIGFPE (test,
// g++ -O0), iako se deli sa -1, a ne nulom.
// Ispravno: poseban slučaj pre deljenja (if (a == INT_MIN && b == -1)),
// ili deljenje u širem tipu.
#include <climits>
#include <iostream>
int main(int argc, char**) {
    int a = INT_MIN;
    int b = -argc;             // -1
    std::cout << a / b << '\n';
}
