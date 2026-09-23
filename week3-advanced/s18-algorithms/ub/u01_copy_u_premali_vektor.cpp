// EXPECT-UB: heap-buffer-overflow
// UB: std::copy (i transform, i svaki algoritam sa izlaznim iteratorom)
// samo PIŠE preko cilja -- ne proverava da li ima mesta i ne povećava
// kontejner. Cilj ima 3 elementa, piše se 5.
// Ispravno: napravi mesto unapred (std::vector<int> cilj(izvor.size());)
// ili pusti da cilj raste: std::copy(..., std::back_inserter(cilj));
#include <algorithm>
#include <iostream>
#include <vector>
int main() {
    std::vector<int> izvor{1, 2, 3, 4, 5};
    std::vector<int> cilj(3);
    std::copy(izvor.begin(), izvor.end(), cilj.begin());
    std::cout << cilj.size() << '\n';
}
