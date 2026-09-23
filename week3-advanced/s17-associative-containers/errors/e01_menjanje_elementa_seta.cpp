// EXPECT-GCC: assignment of read-only location
// EXPECT-CLANG: cannot assign to return value because function 'operator*' returns a const value
// POGREŠNO: element seta (i ključ mape) je const. Njegovo mesto u stablu
// zavisi od vrednosti -- promena bi pokvarila poredak, i set više ne bi
// mogao da ga nađe.
// Ispravno: obriši staru vrednost i ubaci novu (s.erase(it); s.insert(5);),
// ili C++17 extract: auto n = s.extract(it); n.value() = 5; s.insert(std::move(n));
#include <set>
int main() {
    std::set<int> s{1, 2};
    *s.begin() = 5;
    return *s.begin();
}
