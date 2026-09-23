// EXPECT-UB: heap-use-after-free
// UB: erase(it) na listi oslobodi čvor na koji it pokazuje. Ostali
// iteratori liste ostaju važeći (to je prednost liste), ali ovaj -- ne.
// ASan: heap-use-after-free, jer je čvor bio posebna alokacija.
// Ispravno: it = l.erase(it); -- erase vraća iterator na sledeći element
// (lekcija 17, zadatak z3).
#include <iostream>
#include <list>
int main() {
    std::list<int> l{1, 2, 3};
    auto it = l.begin();
    l.erase(it);
    std::cout << *it << '\n';
}
