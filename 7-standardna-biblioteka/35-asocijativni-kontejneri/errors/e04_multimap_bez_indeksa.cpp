// EXPECT-GCC: no match for 'operator[]' (operand types are 'std::multimap<int, int>' and 'int')
// EXPECT-CLANG: type 'std::multimap<int, int>' does not provide a subscript operator
// POGREŠNO: multimap ima više vrednosti za isti ključ -- m[1] ne bi znao
// koju da vrati. (Isto važi za unordered_multimap.)
// Ispravno: m.insert({1, 2}) za dodavanje, m.equal_range(1) za sve
// vrednosti ključa (main.cpp, sekcija 3).
#include <map>
int main() {
    std::multimap<int, int> m;
    m[1] = 2;
    return m.begin()->second;
}
