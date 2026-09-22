// STD: c++17
// EXPECT-GCC: cannot bind rvalue reference of type 'std::vector<int>&&' to lvalue
// EXPECT-CLANG: no matching function for call to 'consume'
// POGREŠNO: std::vector<T>&& očekivan kao forwarding referenca.
// Zašto: forwarding referenca je SAMO oblik "T&&" gde je T parametar
//   šablona koji se dedukuje (EMC Item 24). std::vector<T>&& je obična
//   rvalue referenca na vektor, pa lvalue ne prolazi.
// Ispravno: template <typename V> void consume(V&& v) (pa ograničiti na
//   vektore ako treba), ili consume(std::move(v)) ako funkcija zaista
//   treba da isprazni vektor.
#include <vector>

template <typename T>
void consume(std::vector<T>&& v) { v.clear(); }

int main() {
    std::vector<int> v{1, 2};
    consume(v);
}
