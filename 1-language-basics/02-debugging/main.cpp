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
//   g++ -std=c++17 -g -O0 1-language-basics/02-debugging/main.cpp -o dbg
//   gdb ./dbg

// ---------------------------------------------------------------- 2
int sum(const std::vector<int>& v) {
    int s = 0;
    for (std::size_t i = 0; i < v.size(); ++i) {
        s += v[i];
    }
    return s;
}

double average(const std::vector<int>& v) {
    int s = sum(v);
    return static_cast<double>(s) / static_cast<double>(v.size());
}

// ---------------------------------------------------------------- 7
// static_assert: provera pri KOMPAJLIRANJU -- ako ne važi, nema programa
// (errors/e01). Za pretpostavke o tipovima i platformi.
static_assert(sizeof(int) >= 4, "code assumes at least a 32-bit int");

// assert: provera pri IZVRŠAVANJU, samo u debug build-u. Sa -DNDEBUG ceo
// izraz nestaje -- zato u njemu nikad nema bočnih efekata (zadatak ex2).
double checkedAverage(const std::vector<int>& v) {
    assert(!v.empty() && "average of an empty array makes no sense");
    return average(v);
}

int main() {
    std::vector<int> readings{10, 20, 30, 40};
    double p = average(readings);
    std::cout << "average: " << p << '\n';
    std::cout << "checked average: " << checkedAverage(readings) << '\n';
#ifdef NDEBUG
    std::cout << "NDEBUG: assert is disabled\n";
#else
    std::cout << "assert is enabled\n";
#endif
}
