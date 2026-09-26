#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

// Debugging -- program za vežbu sa gdb-om (notes.md, sekcije 2 i 3) i
// ISPRAVNI primeri assert/static_assert (sekcija 7). Kompajlira se bez
// upozorenja i radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i
// C++20). Brojevi redova u notes.md se odnose na OVAJ fajl.
// POGREŠNI slučajevi (bagovi za hvatanje):
//   ub/      -- ASan/UBSan izveštaji iz sekcija 4-6
//   errors/  -- static_assert i -Werror
// Za gdb, build ručno (build.sh briše izvršni fajl posle pokretanja):
//   g++ -std=c++17 -g -O0 1-osnove-jezika/02-debagovanje/main.cpp -o dbg
//   gdb ./dbg

// ---------------------------------------------------------------- 2
int zbir(const std::vector<int>& v) {
    int s = 0;
    for (std::size_t i = 0; i < v.size(); ++i) {
        s += v[i];
    }
    return s;
}

double prosek(const std::vector<int>& v) {
    int s = zbir(v);
    return static_cast<double>(s) / static_cast<double>(v.size());
}

// ---------------------------------------------------------------- 7
// static_assert: provera pri KOMPAJLIRANJU -- ako ne važi, nema programa
// (errors/e01). Za pretpostavke o tipovima i platformi.
static_assert(sizeof(int) >= 4, "kod pretpostavlja bar 32-bitni int");

// assert: provera pri IZVRŠAVANJU, samo u debug build-u. Sa -DNDEBUG ceo
// izraz nestaje -- zato u njemu nikad nema bočnih efekata (zadatak z2).
double prosekSaProverom(const std::vector<int>& v) {
    assert(!v.empty() && "prosek praznog niza nema smisla");
    return prosek(v);
}

int main() {
    std::vector<int> ocitavanja{10, 20, 30, 40};
    double p = prosek(ocitavanja);
    std::cout << "prosek: " << p << '\n';
    std::cout << "prosek sa proverom: " << prosekSaProverom(ocitavanja) << '\n';
#ifdef NDEBUG
    std::cout << "NDEBUG: assert je isključen\n";
#else
    std::cout << "assert je uključen\n";
#endif
}
