// EXPECT-GCC: no match for 'operator-' (operand types are 'std::_List_iterator<int>' and 'std::_List_iterator<int>')
// EXPECT-CLANG: invalid operands to binary expression ('std::_List_iterator<int>' and 'std::_List_iterator<int>')
// POGREŠNO: std::sort traži RANDOM ACCESS iteratore (last - first, it + n).
// Iterator liste je samo bidirectional (++ i --), pa greška nastaje
// duboko u <algorithm>, gde sort oduzima iteratore.
// Ispravno: l.sort() -- lista ima svoj sort, koji samo prevezuje čvorove
// (main.cpp, sekcija 5). Ili podaci u vektoru, ako se često sortiraju.
#include <algorithm>
#include <list>
int main() {
    std::list<int> l{3, 1, 2};
    std::sort(l.begin(), l.end());
    return l.front();
}
