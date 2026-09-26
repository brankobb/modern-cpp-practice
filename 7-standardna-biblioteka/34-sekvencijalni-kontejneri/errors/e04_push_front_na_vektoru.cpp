// EXPECT-GCC: 'class std::vector<int>' has no member named 'push_front'
// EXPECT-CLANG: no member named 'push_front' in 'std::vector<int>'
// POGREŠNO: vector nema push_front, jer bi to bilo O(n) -- svi elementi
// se pomeraju za jedno mesto. Interfejs nudi samo operacije koje su za taj
// kontejner jeftine.
// Ispravno: std::deque (push_front je O(1)); ili, ako je retko,
// v.insert(v.begin(), x) -- tu se vidi da je umetanje, sa svojom cenom.
#include <vector>
int main() {
    std::vector<int> v{1};
    v.push_front(0);
    return v[0];
}
