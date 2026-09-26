// STD: c++17
// EXPECT-GCC: use of deleted function 'std::unique_ptr<_Tp, _Dp>::unique_ptr(const std::unique_ptr<_Tp, _Dp>&)
// EXPECT-CLANG: call to deleted constructor of 'std::unique_ptr<int>'
// POGREŠNO: kopija unique_ptr.
// Zašto: unique_ptr je JEDINI vlasnik objekta. Kopija bi značila dva
//   vlasnika i dva delete. Zato ima samo move: vlasništvo se prenosi, a
//   izvor postaje nullptr.
// Ispravno: std::unique_ptr<int> b = std::move(a); ili std::shared_ptr ako
//   vlasništvo zaista treba da se deli (lekcija 32).
#include <memory>

int main() {
    auto a = std::make_unique<int>(5);
    std::unique_ptr<int> b = a;
    return *b;
}
