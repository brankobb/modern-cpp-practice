// Rešenje zadatka ex3_auto_unsigned.

#include <iostream>
#include <vector>

// Ako pišeš "for (auto i = v.size() - 1; i >= 0; --i)" (nije dobro): i je
// size_t, uslov je uvek tačan, i petlja čita van niza. Za prazan vektor
// v.size() - 1 je odmah najveći size_t.
//
// Treba ovako: a) reverse iteratori -- nema aritmetike sa indeksom.
void printReversedA(const std::vector<int>& v) {
    std::cout << "a:";
    for (auto it = v.rbegin(); it != v.rend(); ++it) std::cout << ' ' << *it;
    std::cout << '\n';
}

// Možeš i ovako: b) uslov "i-- > 0" proveri PRE umanjenja, pa i nikad ne
// ode ispod nule. Za prazan vektor: 0 > 0 je netačno odmah.
void printReversedB(const std::vector<int>& v) {
    std::cout << "b:";
    for (auto i = v.size(); i-- > 0;) std::cout << ' ' << v[i];
    std::cout << '\n';
}

int main() {
    std::vector<int> v{1, 2, 3};
    std::vector<int> empty;
    printReversedA(v);
    printReversedB(v);
    printReversedA(empty);
    printReversedB(empty);
}
