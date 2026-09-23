// EXPECT-GCC: 'struct std::array<int, 3>' has no member named 'push_back'
// EXPECT-CLANG: no member named 'push_back' in 'std::array<int, 3>'
// POGREŠNO: std::back_inserter(c) na svako upisivanje zove c.push_back().
// std::array ima fiksnu veličinu i nema push_back.
// Ispravno: array već ima mesta, pa se piše direktno u njega:
//   std::copy(v.begin(), v.end(), a.begin());   (ako je v.size() <= a.size())
// ili ciljni kontejner koji raste (vector, deque, list).
#include <algorithm>
#include <array>
#include <iterator>
#include <vector>
int main() {
    std::vector<int> v{1, 2, 3};
    std::array<int, 3> a{};
    std::copy(v.begin(), v.end(), std::back_inserter(a));
    return a[0];
}
