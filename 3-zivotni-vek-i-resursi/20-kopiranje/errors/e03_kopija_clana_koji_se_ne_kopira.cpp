// STD: c++17
// EXPECT-GCC: use of deleted function 'Holder::Holder(const Holder&)'
// EXPECT-CLANG: call to implicitly-deleted copy constructor of 'Holder'
// POGREŠNO: kopija klase koja ima član bez copy konstruktora (unique_ptr).
// Zašto: kompajlerov copy ctor kopira član po član. unique_ptr se ne može
//   kopirati (jedan vlasnik), pa je i Holder(const Holder&) obrisan.
//   Isto važi za std::mutex (lekcija 09, errors/e20), std::thread, fstream.
// Ispravno: move (Holder b = std::move(a);, lekcija 22), ili napiši copy ctor
//   koji pravi NOVI objekat (p(a.p ? std::make_unique<int>(*a.p) : nullptr)).
#include <memory>

struct Holder {
    std::unique_ptr<int> p;
};

int main() {
    Holder a;
    Holder b = a;
    (void)b;
}
