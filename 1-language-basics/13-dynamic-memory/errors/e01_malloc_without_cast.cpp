// STD: c++17
// EXPECT-GCC: invalid conversion from 'void*' to 'int*'
// EXPECT-CLANG: cannot initialize a variable of type 'int *' with an rvalue of type 'void *'
// POGREŠNO: rezultat malloc-a dodeljen int* bez cast-a.
// Zašto: malloc vraća void*. U C-u se void* sam pretvara u bilo koji
//   pokazivač na objekat, a u C++ ne ([conv.ptr] ide samo T* -> void*).
//   C++ to namerno traži eksplicitno, jer void* ne nosi informaciju o tipu.
// Ispravno: static_cast<int*>(std::malloc(n * sizeof(int))). Još bolje:
//   new int[n], std::vector<int>(n) ili std::make_unique<int[]>(n).
#include <cstdlib>

int main() {
    int* p = std::malloc(sizeof(int));
    std::free(p);
}
