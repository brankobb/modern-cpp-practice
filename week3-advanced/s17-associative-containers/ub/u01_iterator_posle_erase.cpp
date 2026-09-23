// EXPECT-UB: heap-use-after-free
// UB: erase oslobodi čvor mape, pa iterator koji je na njega pokazivao
// visi. Ostali iteratori mape ostaju važeći (čvorovi se ne premeštaju).
// Ispravno: ne koristi iterator obrisanog elementa. Kad brišeš u petlji:
//   it = m.erase(it);   (C++11: erase vraća iterator na sledeći)
#include <iostream>
#include <map>
int main() {
    std::map<int, int> m{{1, 10}, {2, 20}};
    auto it = m.find(2);
    m.erase(2);
    std::cout << it->second << '\n';
}
