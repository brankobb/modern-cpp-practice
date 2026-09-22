// STD: c++17
// EXPECT-GCC: array subscript value '3' is outside the bounds of array 'table'
// EXPECT-CLANG: constexpr variable 'v' must be initialized by a constant expression
// POGREŠNO: pristup van niza u constexpr izračunavanju.
// Zašto: isto kao e01 -- UB nije dozvoljen u konstantnom izrazu, pa je
//   greška pri kompajliranju. Pri izvršavanju isti poziv je tihi UB (ub/u02).
// Ispravno: indeks 0..2; ili std::array i .at(i) (u constexpr kontekstu
//   izuzetak iz at() je takođe greška pri kompajliranju).
constexpr int table[3] = {10, 20, 30};

constexpr int at(int i) { return table[i]; }

constexpr int v = at(3);

int main() {
    return v;
}
