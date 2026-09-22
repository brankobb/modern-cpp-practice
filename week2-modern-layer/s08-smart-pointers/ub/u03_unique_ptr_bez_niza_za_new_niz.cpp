// EXPECT-UB: alloc-dealloc-mismatch \(operator new \[\] vs operator delete\)
// POGREŠNO: std::unique_ptr<int> (bez []) drži memoriju od new int[5].
// Zašto: unique_ptr<int> briše sa delete, a niz traži delete[] (lekcija 10,
//   EC++ Item 16). Tip unique_ptr-a ne može da zna odakle je pokazivač.
// Ispravno: std::unique_ptr<int[]> ili std::make_unique<int[]>(5)
//   (main.cpp, sekcija 7). make_unique bira pravi oblik sam.
#include <cstdio>
#include <memory>

int main() {
    std::unique_ptr<int> p(new int[5]{});
    std::printf("%d\n", *p);
}
