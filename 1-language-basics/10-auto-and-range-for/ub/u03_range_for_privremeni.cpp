// EXPECT-UB: stack-use-after-scope|heap-use-after-free
// UB (do C++23): range-for produžava život samo POSLEDNJEG privremenog
// objekta u izrazu. Holder{} je privremen, items() vraća referencu na njegov
// član -- Holder nestaje pre prve iteracije.
// C++23 (P2718) ovo popravlja; u C++17/C++20 je UB.
// Ispravno: Holder h; for (int x : h.items()) ...
#include <iostream>
#include <vector>
struct Holder {
    std::vector<int> data{1, 2, 3};
    const std::vector<int>& items() const { return data; }
};
int main() {
    for (int x : Holder{}.items()) std::cout << x << "\n";
}
