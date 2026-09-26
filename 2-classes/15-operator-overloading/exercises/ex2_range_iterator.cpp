// KIND: usage
//
// Zadatak 2 -- sopstveni iterator: *, ++, != i range-for; operator[] i
// operator() (sekcije 7, 8, 9)
//   ./build.sh 2-classes/15-operator-overloading/exercises/ex2_range_iterator.cpp
// Rešenje: exercises/solutions/ex2_range_iterator.cpp
//
// Korak 1: class Range(int first, int last) predstavlja brojeve [first, last).
//   Unutar nje class Iterator sa int i_: int operator*() const,
//   Iterator& operator++() (prefiks), Iterator operator++(int) (postfiks,
//   vraća STARU vrednost), operator== i operator!=. Range ima begin() i
//   end(). Range-for "for (int x : range)" tada radi sam -- kompajler ga
//   prevodi u begin(), end(), !=, ++ i *.
// Korak 2: int operator[](int k) const vraća k-ti broj u opsegu, a za k van
//   opsega baca std::out_of_range.
// Korak 3: funkcijski objekat class Scale sa int factor_ i
//   int operator()(int x) const. Upotrebi ga sa std::transform.

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // const char* sep = "";
    // for (int x : Range(1, 5)) {
    //     std::cout << sep << x;
    //     sep = " ";
    // }
    // std::cout << '\n';
    // Range r(1, 5);
    // auto it = r.begin();
    // int old = *it++;
    // std::cout << "it++ returns the old value: " << old << ", now: " << *it << '\n';

    // Korak 2 -- otkomentariši:
    // std::cout << "Range(10, 20)[3] = " << Range(10, 20)[3] << '\n';
    // try {
    //     Range(10, 20)[10];
    // } catch (const std::out_of_range&) {
    //     std::cout << "[10]: out_of_range\n";
    // }

    // Korak 3 -- otkomentariši:
    // std::vector<int> v;
    // for (int x : Range(1, 5)) v.push_back(x);
    // std::transform(v.begin(), v.end(), v.begin(), Scale{3});
    // std::cout << "scaled:";
    // for (int x : v) std::cout << ' ' << x;
    // std::cout << '\n';
}

/* EXPECTED OUTPUT
1 2 3 4
it++ returns the old value: 1, now: 2
Range(10, 20)[3] = 13
[10]: out_of_range
scaled: 3 6 9 12
*/
