// EXPECT-GCC: need 'typename' before 'C::value_type' because 'C' is a dependent scope
// EXPECT-CLANG: missing 'typename' prior to dependent type name 'C::value_type'
// POGREŠNO: C::value_type zavisi od parametra šablona. Pri prvoj proveri
// šablona (two-phase lookup, lekcija 26) kompajler ne zna da li je to TIP ili
// statički član, i pretpostavi da nije tip.
// Ispravno: typename C::value_type x = c[0]; -- "typename" kaže "ovo je
// tip". (Ili auto x = c[0];)
#include <vector>
template <typename C>
int prvi(const C& c) {
    C::value_type x = c[0];
    return static_cast<int>(x);
}
int main() {
    std::vector<int> v{1};
    return prvi(v);
}
