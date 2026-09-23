// EXPECT-GCC: use of deleted function 'std::unique_ptr<_Tp, _Dp>::unique_ptr(const std::unique_ptr<_Tp, _Dp>&)
// EXPECT-CLANG: call to deleted constructor of
// POGREŠNO: [p] zarobljava KOPIJU -- a unique_ptr nema kopiju.
// Ispravno: generalizovani capture (C++14) premesti vlasništvo u lambdu:
//   [q = std::move(p)] { return *q; }   (sekcija 8)
// Posle toga je p prazan, a lambda je vlasnik.
#include <memory>
int main() {
    auto p = std::make_unique<int>(7);
    auto f = [p] { return *p; };
    return f();
}
