// EXPECT-UB: reference binding to null pointer
// POGREŠNO: front() na praznom vektoru.
// Zašto: front() je *begin(); kod praznog vektora nema elementa, a begin()
//   je ovde nullptr. isto važi za back() i pop_back(). Nijedna od tih
//   funkcija ne proverava (za razliku od at()).
// Ispravno: if (!v.empty()) pre front()/back()/pop_back().
#include <cstdio>
#include <vector>

int main() {
    std::vector<int> v;
    int first = v.front();
    std::printf("%d\n", first);
}
