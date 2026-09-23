// VRSTA: upotreba
//
// Zadatak 2 -- sopstveni iterator: *, ++, != i range-for; operator[] i
// operator() (sekcije 7, 8, 9)
//   ./build.sh week0-fundamentals/12-operator-overloading/exercises/z2_opseg_iterator.cpp
// Rešenje: exercises/solutions/z2_opseg_iterator.cpp
//
// Korak 1: class Opseg(int pocetak, int kraj) predstavlja brojeve [pocetak, kraj).
//   Unutar njega class Iterator sa int i_: int operator*() const,
//   Iterator& operator++() (prefiks), Iterator operator++(int) (postfiks,
//   vraća STARU vrednost), operator== i operator!=. Opseg ima begin() i
//   end(). Range-for "for (int x : opseg)" tada radi sam -- kompajler ga
//   prevodi u begin(), end(), !=, ++ i *.
// Korak 2: int operator[](int k) const vraća k-ti broj u opsegu, a za k van
//   opsega baca std::out_of_range.
// Korak 3: funkcijski objekat class Skaliraj sa int faktor_ i
//   int operator()(int x) const. Upotrebi ga sa std::transform.

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // const char* sep = "";
    // for (int x : Opseg(1, 5)) {
    //     std::cout << sep << x;
    //     sep = " ";
    // }
    // std::cout << '\n';
    // Opseg o(1, 5);
    // auto it = o.begin();
    // int stara = *it++;
    // std::cout << "it++ vraća staru: " << stara << ", sada: " << *it << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "Opseg(10, 20)[3] = " << Opseg(10, 20)[3] << '\n';
    // try {
    //     Opseg(10, 20)[10];
    // } catch (const std::out_of_range&) {
    //     std::cout << "[10]: out_of_range\n";
    // }

    // Korak 3 -- otkomentariši:
    // std::vector<int> v;
    // for (int x : Opseg(1, 5)) v.push_back(x);
    // std::transform(v.begin(), v.end(), v.begin(), Skaliraj{3});
    // std::cout << "skalirano:";
    // for (int x : v) std::cout << ' ' << x;
    // std::cout << '\n';
}

/* OČEKIVANI IZLAZ
1 2 3 4
it++ vraća staru: 1, sada: 2
Opseg(10, 20)[3] = 13
[10]: out_of_range
skalirano: 3 6 9 12
*/
