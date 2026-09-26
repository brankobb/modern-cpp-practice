// EXPECT-UB: heap-use-after-free
// POGREŠNO: push_back u vektor preko kog ide range-for petlja.
// Zašto: range-for je petlja sa iteratorima (begin()/end() izračunati
//   jednom, lekcija 10). push_back može da realocira, pa iterator petlje
//   pokazuje u stari, oslobođen niz.
// Ispravno: skupi nove elemente u drugi vektor pa ih dodaj posle petlje,
//   ili petlja sa indeksom (for (std::size_t i = 0; i < v.size(); ++i)).
#include <cstdio>
#include <vector>

int main() {
    std::vector<int> v{1, 2, 3, 4};
    for (int x : v) {
        if (x == 2) v.push_back(99);
    }
    std::printf("%zu\n", v.size());
}
