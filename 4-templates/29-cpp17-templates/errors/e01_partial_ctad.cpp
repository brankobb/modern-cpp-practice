// EXPECT-GCC: wrong number of template arguments (1, should be 2)
// EXPECT-CLANG: too few template arguments for class template 'pair'
// POGREŠNO: CTAD je sve ili ništa. Kad navedeš bar jedan argument
// šablona, dedukcija se ne radi, i moraš da navedeš sve (koji nemaju
// podrazumevanu vrednost).
// Ispravno: std::pair p{1, 2};  ili  std::pair<int, int> p{1, 2};
#include <utility>
int main() {
    std::pair<int> p{1, 2};
    return p.first;
}
