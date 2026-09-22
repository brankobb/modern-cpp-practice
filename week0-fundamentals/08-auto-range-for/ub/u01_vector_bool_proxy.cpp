// EXPECT-UB: heap-use-after-free
// UB (EMC Item 6): auto dedukuje std::vector<bool>::reference -- proxy koji
// pokazuje u memoriju PRIVREMENOG vektora. Vektor nestaje na kraju izraza,
// a proxy i dalje pokazuje na oslobođenu memoriju.
// Ispravno: bool highPriority = features()[5];
//       ili auto highPriority = static_cast<bool>(features()[5]);  (EMC Item 6)
#include <iostream>
#include <vector>
std::vector<bool> features() {
    return std::vector<bool>(100, true);
}
int main() {
    auto highPriority = features()[5];
    std::cout << highPriority << "\n";
}
