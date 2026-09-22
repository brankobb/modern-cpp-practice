// STD: c++17
// EXPECT-GCC: no match for 'operator*'
// EXPECT-CLANG: indirection requires pointer operand
// POGREŠNO: *p za std::unique_ptr<int[]>.
// Zašto: verzija za niz (unique_ptr<T[]>) namerno nema operator* ni
//   operator->, nego operator[]. "*p" bi bio samo prvi element, a to je
//   najčešće greška u razmišljanju (niz tretiran kao jedan objekat).
// Ispravno: p[0]. Za niz koji menja veličinu: std::vector.
#include <memory>

int main() {
    auto p = std::make_unique<int[]>(3);
    return *p;
}
