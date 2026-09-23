// EXPECT-GCC: class template argument deduction failed
// EXPECT-CLANG: no viable constructor or deduction guide for deduction of template arguments of 'Par'
// POGREŠNO (u C++17): CTAD čita tipove iz KONSTRUKTORA. Agregat nema
// konstruktor, pa u C++17 nema odakle da zaključi T. C++20 dodaje
// dedukciju za agregate i ovo se tamo kompajlira.
// Ispravno u C++17: deduction guide uz šablon:
//   template <typename T> Par(T, T) -> Par<T>;   (main.cpp, sekcija 1)
#include <iostream>
template <typename T>
struct Par {
    T prvi, drugi;
};
int main() {
    Par p{1, 2};
    std::cout << p.prvi + p.drugi << '\n';
}
