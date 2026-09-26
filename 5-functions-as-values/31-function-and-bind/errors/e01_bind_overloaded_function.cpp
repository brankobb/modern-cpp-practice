// EXPECT-GCC: no matching function for call to 'bind(<unresolved overloaded function type>
// EXPECT-CLANG: no matching function for call to 'bind'
// POGREŠNO: add je PREOPTEREĆENA (int i double verzija). bind je šablon
// koji prima "bilo šta", pa nema na osnovu čega da izabere overload --
// ime nema jedan tip.
// Ispravno: lambda, gde se overload bira normalno, na mestu poziva:
//   auto f = [](int x) { return add(x, 1); };
// ili cast na tačan tip: std::bind(static_cast<int (*)(int, int)>(add), _1, 1).
#include <functional>
int add(int a, int b) { return a + b; }
double add(double a, double b) { return a + b; }
int main() {
    using namespace std::placeholders;
    auto f = std::bind(add, _1, 1);
    return f(2);
}
