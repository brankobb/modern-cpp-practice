// STD: c++17
// EXPECT-GCC: cannot bind non-const lvalue reference of type 'std::_Bit_reference&'
// EXPECT-CLANG: cannot bind to a temporary of type 'reference'
// POGREŠNO: std::vector<bool> ne čuva bool-ove nego bitove, pa *it vraća
// privremeni PROXY objekat (std::vector<bool>::reference), a auto& ne može
// da se veže za privremeni objekat.
// Ispravno: for (auto&& b : flags)  -- ili for (bool b : flags) za čitanje
#include <vector>
int main() {
    std::vector<bool> flags{true, false};
    for (auto& b : flags) b = !b;
}
