// EXPECT-GCC: class template argument deduction failed
// EXPECT-CLANG: no viable constructor or deduction guide for deduction of template arguments of 'Pair'
// POGREŠNO (u C++17): CTAD čita tipove iz KONSTRUKTORA. Agregat nema
// konstruktor, pa u C++17 nema odakle da zaključi T. C++20 dodaje
// dedukciju za agregate i ovo se tamo kompajlira.
// Ispravno u C++17: deduction guide uz šablon:
//   template <typename T> Pair(T, T) -> Pair<T>;   (main.cpp, sekcija 1)
#include <iostream>
template <typename T>
struct Pair {
    T first, second;
};
int main() {
    Pair p{1, 2};
    std::cout << p.first + p.second << '\n';
}
