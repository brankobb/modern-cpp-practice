// EXPECT-GCC: no matching function for call to 'accumulate(std::vector<int>&, int)'
// EXPECT-CLANG: no matching function for call to 'accumulate'
// POGREŠNO: klasični algoritmi (<algorithm>, <numeric>) primaju OPSEG --
// par iteratora -- a ne kontejner. Tako rade i za C niz, deo kontejnera
// i bilo šta sa iteratorima.
// Ispravno: std::accumulate(v.begin(), v.end(), 0);
// (C++20 std::ranges:: algoritmi primaju i ceo kontejner, npr.
//  std::ranges::sort(v); za accumulate ranges verzije nema do C++23.)
#include <numeric>
#include <vector>
int main() {
    std::vector<int> v{1, 2, 3};
    int zbir = std::accumulate(v, 0);
    return zbir;
}
