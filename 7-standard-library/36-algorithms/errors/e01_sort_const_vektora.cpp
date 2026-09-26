// EXPECT-GCC: assignment of read-only location
// EXPECT-CLANG: cannot assign to return value because function 'operator*' returns a const value
// POGREŠNO: std::sort premešta elemente kroz iteratore. begin() const
// vektora vraća const_iterator, pa algoritam ne sme da piše -- greška
// se prijavi duboko u <algorithm>, ne na liniji poziva.
// Ispravno: sortiraj vektor koji nije const, ili kopiju:
//   auto kopija = v; std::sort(kopija.begin(), kopija.end());
#include <algorithm>
#include <vector>
int main() {
    const std::vector<int> v{3, 1, 2};
    std::sort(v.begin(), v.end());
    return v[0];
}
