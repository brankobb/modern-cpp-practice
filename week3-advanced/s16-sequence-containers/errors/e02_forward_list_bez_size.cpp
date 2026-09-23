// EXPECT-GCC: 'class std::forward_list<int>' has no member named 'size'
// EXPECT-CLANG: no member named 'size' in 'std::forward_list<int>'
// POGREŠNO: forward_list namerno nema size(): čuva samo pokazivač na prvi
// čvor (sizeof je 8 bajtova), a brojač bi koštao mesto i vreme pri svakoj
// izmeni. Cilj mu je da bude najmanja moguća lista.
// Ispravno: std::distance(f.begin(), f.end()) -- O(n); ili std::list,
// koja ima size() u O(1).
#include <forward_list>
int main() {
    std::forward_list<int> f{1, 2};
    return static_cast<int>(f.size());
}
