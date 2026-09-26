// EXPECT-GCC: assignment of read-only location
// EXPECT-CLANG: cannot assign to return value because function 'operator*' returns a const value
// POGREŠNO: std::remove ne briše, nego PREPISUJE elemente koji ostaju ka
// početku opsega. Elementi seta su const (njihovo mesto u stablu zavisi
// od vrednosti, lekcija 35), pa se ne mogu prepisati.
// Ispravno: kontejner koji zna da briše sam: s.erase(2);
// (C++20: std::erase_if(s, pred) radi i za set.)
#include <algorithm>
#include <set>
int main() {
    std::set<int> s{1, 2, 3};
    std::remove(s.begin(), s.end(), 2);
    return static_cast<int>(s.size());
}
