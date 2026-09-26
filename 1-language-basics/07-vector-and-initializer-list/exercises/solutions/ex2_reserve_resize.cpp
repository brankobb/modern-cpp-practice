// Rešenje zadatka ex2_reserve_resize.

#include <cstddef>
#include <iostream>
#include <vector>

// Ako posle reserve(n) pišeš v[i] = ... (nije dobro): elementi ne postoje,
// size ostane 0, a upis je UB koji običan ASan ne vidi (memorija je
// zauzeta).
// Treba ovako: a) reserve samo da izbegneš realokacije, a elemente dodaj
// sa push_back / emplace_back.
std::vector<int> readSamplesA(std::size_t n) {
    std::vector<int> v;
    v.reserve(n);
    for (std::size_t i = 0; i < n; ++i) v.push_back(static_cast<int>(i * i));
    return v;
}

// Možeš i ovako: b) resize(n) napravi n elemenata (nule), pa ih menjaš.
std::vector<int> readSamplesB(std::size_t n) {
    std::vector<int> v;
    v.resize(n);
    for (std::size_t i = 0; i < n; ++i) v[i] = static_cast<int>(i * i);
    return v;
}

int main() {
    for (const auto& v : {readSamplesA(4), readSamplesB(4)}) {
        std::cout << "size=" << v.size() << ':';
        for (int x : v) std::cout << ' ' << x;
        std::cout << '\n';
    }
}
