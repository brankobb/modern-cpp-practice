// STD: c++17
// EXPECT-GCC: TD<const int&>
// EXPECT-CLANG: TD<const int &>
// NAMERNA GREŠKA (EMC Item 4): najpouzdaniji način da vidiš koji tip je
// kompajler dedukovao -- deklariši šablon bez definicije i upotrebi ga.
// Poruka o grešci ispiše tačan tip: ovde TD<const int&>.
template <typename T>
class TD;
int main() {
    const int x = 0;
    auto& y = x;
    TD<decltype(y)> show;
}
