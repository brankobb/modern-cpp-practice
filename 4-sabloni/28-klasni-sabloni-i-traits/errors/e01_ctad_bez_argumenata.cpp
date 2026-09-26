// EXPECT-GCC: class template argument deduction failed
// EXPECT-CLANG: no viable constructor or deduction guide for deduction of template arguments of 'vector'
// POGREŠNO: CTAD (C++17) izvodi argumente šablona iz argumenata
// KONSTRUKTORA. "std::vector v;" nema nijedan argument -- nema iz čega da
// se izvede T.
// Ispravno: std::vector<int> v; ili std::vector v{1, 2, 3}; (T = int iz
// liste). Oprez: std::vector v{5} je vektor sa JEDNIM elementom 5 (lekcija 03).
#include <vector>
int main() {
    std::vector v;
    return static_cast<int>(v.size());
}
