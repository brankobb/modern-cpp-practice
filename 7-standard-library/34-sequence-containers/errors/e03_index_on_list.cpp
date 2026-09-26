// EXPECT-GCC: no match for 'operator[]' (operand types are 'std::__cxx11::list<int>' and 'int')
// EXPECT-CLANG: type 'std::list<int>' does not provide a subscript operator
// POGREŠNO: lista nema operator[] -- do n-tog elementa se stiže
// prolaskom kroz n čvorova (O(n)), a biblioteka ne nudi operaciju koja bi
// izgledala jeftino, a bila spora.
// Ispravno: *std::next(l.begin(), 1) (vidi se da je to hod kroz listu), ili
// vector/deque ako je pristup po indeksu čest.
#include <list>
int main() {
    std::list<int> l{1, 2};
    return l[1];
}
