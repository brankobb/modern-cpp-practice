// STD: c++17
// EXPECT-GCC: no matching function for call to 'forwardTo(<brace-enclosed initializer list>)'
// EXPECT-CLANG: no matching function for call to 'forwardTo'
// POGREŠNO: {1, 2, 3} prosleđeno kroz šablon sa forwarding referencom.
// Zašto: {1, 2, 3} nema tip, pa T ne može da se dedukuje ([temp.deduct.call]).
//   Direktan poziv take({1, 2, 3}) radi, jer tamo parametar ima konkretan
//   tip. Ovo je prvi od slučajeva gde "savršeno" prosleđivanje nije
//   savršeno (EMC Item 30); isto važi za emplace_back({1, 2}).
// Ispravno: forwardTo(std::vector<int>{1, 2, 3}), ili
//   auto list = {1, 2, 3}; (std::initializer_list<int>) pa forwardTo(list).
#include <utility>
#include <vector>

void take(const std::vector<int>& v) { (void)v; }

template <typename T>
void forwardTo(T&& arg) { take(std::forward<T>(arg)); }

int main() {
    take({1, 2, 3});
    forwardTo({1, 2, 3});
}
