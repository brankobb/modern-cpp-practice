// EXPECT-UB: signed integer overflow: 2147483647 \+ 1 cannot be represented in type 'int'
// UB: prekoračenje signed int-a ([expr.pre]). Nije "wrap" na -2147483648 --
// kompajler sme da pretpostavi da se ne dešava (npr. da x + 1 > x uvek važi).
// Ispravno: proveri pre operacije (x > INT_MAX - 1), ili računaj u širem
// tipu (long long), ili u unsigned ako ti treba modulo (main.cpp, sekcija 4).
#include <climits>
#include <iostream>
int main(int argc, char**) {
    int x = INT_MAX;
    int y = x + argc;      // argc == 1
    std::cout << y << '\n';
}
