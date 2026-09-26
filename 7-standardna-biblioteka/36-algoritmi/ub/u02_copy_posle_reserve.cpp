// EXPECT-UB: container-overflow
// FLAGS: -D_GLIBCXX_SANITIZE_VECTOR
// UB: reserve menja KAPACITET, ne veličinu. Posle reserve(5) vektor je i
// dalje prazan; copy upiše 5 brojeva u memoriju koja nije element, a
// size() ostane 0 -- vrednosti su "nevidljive".
// Bez -D_GLIBCXX_SANITIZE_VECTOR ASan ovo NE vidi: memorija pripada
// bloku koji je vektor alocirao, pa program tiho ispiše 0. Makro kaže
// libstdc++-u da označi deo između size() i capacity() kao zabranjen.
// Ispravno: std::copy(izvor.begin(), izvor.end(), std::back_inserter(cilj));
// (reserve pre toga samo uštedi realokacije).
#include <algorithm>
#include <iostream>
#include <vector>
int main() {
    std::vector<int> izvor{1, 2, 3, 4, 5};
    std::vector<int> cilj;
    cilj.reserve(5);
    std::copy(izvor.begin(), izvor.end(), cilj.begin());
    std::cout << cilj.size() << '\n';
}
