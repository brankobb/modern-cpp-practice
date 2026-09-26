// EXPECT-UB: caused by a WRITE memory access
// UB: const_cast sme da skine const, ali menjanje objekta koji je STVARNO
// definisan kao const je UB. Globalni const objekat je obično u memoriji
// koja je samo za čitanje, pa upis sruši program.
// const_cast je legitiman samo kad je originalni objekat ne-const (main.cpp, sekcija 7).
#include <iostream>
const int limit = 10;
int main() {
    int* p = const_cast<int*>(&limit);
    *p = 20;
    std::cout << limit << "\n";
}
