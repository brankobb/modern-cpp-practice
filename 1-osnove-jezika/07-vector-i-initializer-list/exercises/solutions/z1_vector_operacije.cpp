// Rešenje zadatka z1_vector_operacije.

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

void ispisi(const char* opis, const std::vector<int>& v) {
    std::cout << opis << ':';
    for (int x : v) std::cout << ' ' << x;
    std::cout << '\n';
}

// Korak 1: jedan prolaz, O(n). Brisanje jedan po jedan sa erase(it) bi
// svaki put pomeralo ostatak niza -- O(n^2) (i lako se pogreši, z3).
void ukloniNegativne(std::vector<int>& v) {
    v.erase(std::remove_if(v.begin(), v.end(), [](int x) { return x < 0; }), v.end());
}

// Korak 2: reserve unapred -> jedna alokacija za ceo rezultat.
std::vector<int> spoji(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> r;
    r.reserve(a.size() + b.size());
    r.insert(r.end(), a.begin(), a.end());
    r.insert(r.end(), b.begin(), b.end());
    return r;
}

// Korak 3: promena data() = novi blok = realokacija (i nevažeći stari
// pokazivači, iteratori i reference).
int brojRealokacija(std::size_t n, bool saReserve) {
    std::vector<int> v;
    if (saReserve) v.reserve(n);
    int broj = 0;
    const int* prethodni = v.data();
    for (std::size_t i = 0; i < n; ++i) {
        v.push_back(static_cast<int>(i));
        if (v.data() != prethodni) {
            ++broj;
            prethodni = v.data();
        }
    }
    // Bez reserve se prvi blok alocira tek pri prvom push_back-u, pa se i
    // to broji kao promena; sa reserve je blok već tu.
    return broj;
}

int main() {
    std::vector<int> v{3, -1, 4, -1, -5, 9};
    ukloniNegativne(v);
    ispisi("bez negativnih", v);

    ispisi("spojeno", spoji({1, 2}, {3, 4, 5}));

    std::cout << "bez reserve realocira: " << (brojRealokacija(1000, false) > 0 ? "da" : "ne") << '\n';
    std::cout << "sa reserve realocira: " << (brojRealokacija(1000, true) > 0 ? "da" : "ne") << '\n';
}
