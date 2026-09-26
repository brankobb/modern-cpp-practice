// STD: c++17
// EXPECT-GCC: cannot bind rvalue reference of type 'std::vector<int>&&' to lvalue
// EXPECT-CLANG: no matching function for call to 'f'
// POGREŠNO: T&& je "univerzalna" (forwarding) referenca SAMO u obliku T&& gde
// se T dedukuje. std::vector<T>&& je obična rvalue referenca, pa ne prima
// lvalue (EMC Item 24; vidi i main.cpp, sekcija 2).
// Ispravno: f(std::move(v))  ili  template <typename T> void f(T&& param)
#include <vector>
template <typename T>
void f(std::vector<T>&&) {}
int main() {
    std::vector<int> v{1};
    f(v);
}
