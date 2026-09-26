// STD: c++17
// EXPECT-GCC: use of deleted function 'std::unique_ptr<_Tp, _Dp>::unique_ptr(const std::unique_ptr<_Tp, _Dp>&)
// EXPECT-CLANG: call to deleted constructor of 'std::unique_ptr<int>'
// POGREŠNO: move-only objekat (lvalue) prosleđen funkciji koja ga prima
//   po vrednosti.
// Zašto: parametar po vrednosti se pravi kopijom kad je argument lvalue.
//   Za unique_ptr kopije nema, pa pozivalac mora eksplicitno da preda
//   vlasništvo. To je dobro: iz koda adopt(all, std::move(p)) se vidi da p
//   posle poziva više nije vlasnik.
// Ispravno: adopt(all, std::move(p));
#include <memory>
#include <utility>
#include <vector>

void adopt(std::vector<std::unique_ptr<int>>& all, std::unique_ptr<int> p) {
    all.push_back(std::move(p));
}

int main() {
    std::vector<std::unique_ptr<int>> all;
    auto p = std::make_unique<int>(3);
    adopt(all, p);
}
