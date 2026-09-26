// STD: c++17
// EXPECT-GCC: assignment of read-only location
// EXPECT-CLANG: returns a const value
// POGREŠNO: const_iterator (cbegin) daje samo čitanje (EMC Item 13).
#include <vector>
int main() {
    std::vector<int> v{1, 2, 3};
    auto it = v.cbegin();
    *it = 5;
}
