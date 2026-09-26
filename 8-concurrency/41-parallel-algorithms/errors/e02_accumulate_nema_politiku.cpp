// EXPECT-GCC: no matching function for call to 'accumulate(const __pstl::execution::v1::parallel_policy&
// EXPECT-CLANG: no matching function for call to 'accumulate'
// POGREŠNO: accumulate je po definiciji s leva na desno, jedan po jedan
// -- ne može paralelno, pa nema verziju sa politikom. (Isto partial_sum,
// inner_product, iota.)
// Ispravno: std::reduce(std::execution::par, v.begin(), v.end(), 0) --
// ali samo za asocijativnu i komutativnu operaciju (sekcija 4; zadatak ex2).
#include <execution>
#include <numeric>
#include <vector>
int main() {
    std::vector<int> v{1, 2, 3};
    return std::accumulate(std::execution::par, v.begin(), v.end(), 0);
}
