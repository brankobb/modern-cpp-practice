// EXPECT-GCC: ambiguous template instantiation for 'struct Pair<int*, int*>'
// EXPECT-CLANG: ambiguous partial specializations of 'Pair<int *, int *>'
// POGREŠNO: za Pair<int*, int*> odgovaraju OBE delimične specijalizacije
// (A = int iz prve, B = int iz druge), a nijedna nije specijalnija od
// druge. Kompajler ne bira "prvu napisanu" -- to je greška.
// Ispravno: dodaj specijalizaciju koja pokriva presek, Pair<A*, B*>; ona
// je specijalnija od obe i pobeđuje.
template <typename A, typename B>
struct Pair {
    static constexpr int v = 0;
};
template <typename A, typename B>
struct Pair<A*, B> {
    static constexpr int v = 1;
};
template <typename A, typename B>
struct Pair<A, B*> {
    static constexpr int v = 2;
};
int main() { return Pair<int*, int*>::v; }
