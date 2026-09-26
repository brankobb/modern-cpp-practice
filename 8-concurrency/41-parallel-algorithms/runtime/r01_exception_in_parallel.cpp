// EXPECT-RUN: terminate called after throwing an instance of 'std::invalid_argument'
// POGREŠNO: kad funkcija koju paralelni algoritam poziva baci izuzetak,
// standard traži std::terminate ([algorithms.parallel.exceptions]) --
// i kad je poziv u try bloku. Izuzetak iz više niti odjednom ne bi imao
// jedan jasan put nazad. Sa sekvencijalnim for_each (bez politike) isti
// izuzetak bi bio uhvaćen. (Provereno i sa TBB-om i bez njega.)
// Ispravno: proveri podatke pre algoritma (none_of / find_if), ili
// hvataj izuzetak UNUTAR lambde i zabeleži grešku kao rezultat.
#include <algorithm>
#include <execution>
#include <iostream>
#include <stdexcept>
#include <vector>
int main() {
    std::vector<int> v{1, 2, -3, 4};
    try {
        std::for_each(std::execution::par, v.begin(), v.end(), [](int x) {
            if (x < 0) throw std::invalid_argument("negative reading");
        });
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << '\n';   // nikad se ne izvrši
    }
}
