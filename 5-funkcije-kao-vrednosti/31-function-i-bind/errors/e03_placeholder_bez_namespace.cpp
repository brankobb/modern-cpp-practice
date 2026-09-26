// EXPECT-GCC: '_1' was not declared in this scope
// EXPECT-CLANG: use of undeclared identifier '_1'
// POGREŠNO: _1, _2... su u namespace-u std::placeholders.
// Ispravno: using namespace std::placeholders; (u funkciji, ne u header-u),
// ili std::placeholders::_1. Lambda placeholder-e uopšte ne treba.
#include <functional>
int puta(int a, int b) { return a * b; }
int main() {
    auto f = std::bind(puta, _1, 2);
    return f(3);
}
