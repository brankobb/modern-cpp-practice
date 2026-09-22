// STD: c++17
// EXPECT-GCC: no match for 'operator*' (operand type is 'std::weak_ptr<int>')
// EXPECT-CLANG: indirection requires pointer operand ('std::weak_ptr<int>' invalid)
// POGREŠNO: *w za std::weak_ptr.
// Zašto: weak_ptr ne drži objekat živim, pa objekat može da nestane u bilo
//   kom trenutku (i iz druge niti). Direktan pristup bi bio "proveri pa
//   koristi" sa trkom između. Zato jedini put do objekta ide kroz lock(),
//   koji atomski napravi shared_ptr (vlasnika za vreme upotrebe) ili prazan.
// Ispravno: if (auto s = w.lock()) { return *s; }
#include <memory>

int main() {
    auto s = std::make_shared<int>(5);
    std::weak_ptr<int> w = s;
    return *w;
}
