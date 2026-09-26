// EXPECT-UB: heap-use-after-free
// UB: pokazivač na element vector-a posle realokacije. push_back iznad
// kapaciteta seli sve elemente u nov blok memorije, a stari oslobađa.
// Ispravno: čuvaj INDEKS umesto pokazivača, ili ponovo uzmi &v[0] posle
// push_back, ili v.reserve(n) unapred.
#include <iostream>
#include <vector>
int main() {
    std::vector<int> v{1, 2, 3};
    int* first = &v[0];
    for (int i = 0; i < 100; ++i) v.push_back(i);
    std::cout << *first << "\n";
}
