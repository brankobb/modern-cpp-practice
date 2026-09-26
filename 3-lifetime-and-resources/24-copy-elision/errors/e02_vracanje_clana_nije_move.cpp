// STD: c++17
// EXPECT-GCC: use of deleted function 'std::unique_ptr<_Tp, _Dp>::unique_ptr(const std::unique_ptr<_Tp, _Dp>&)
// EXPECT-CLANG: call to deleted constructor of 'std::unique_ptr<int>'
// POGREŠNO: "return data_;" gde je data_ član, a tip move-only.
// Zašto: automatski move na return važi samo za LOKALNE promenljive i
//   parametre funkcije -- objekte koji ionako nestaju na kraju funkcije.
//   Član objekta i dalje živi, pa se vraća KOPIJA, a unique_ptr nema kopiju.
// Ispravno: return std::move(data_); -- ovde je std::move na mestu, jer
//   namerno prazniš član (vlasništvo odlazi pozivaocu). Ili
//   std::exchange(data_, nullptr).
#include <memory>

class Cache {
public:
    std::unique_ptr<int> take() { return data_; }

private:
    std::unique_ptr<int> data_ = std::make_unique<int>(1);
};

int main() {
    Cache c;
    return *c.take();
}
