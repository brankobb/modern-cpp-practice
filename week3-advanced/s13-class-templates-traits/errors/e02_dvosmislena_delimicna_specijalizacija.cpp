// EXPECT-GCC: ambiguous template instantiation for 'struct Par<int*, int*>'
// EXPECT-CLANG: ambiguous partial specializations of 'Par<int *, int *>'
// POGREŠNO: za Par<int*, int*> odgovaraju OBE delimične specijalizacije
// (A = int iz prve, B = int iz druge), a nijedna nije specijalnija od
// druge. Kompajler ne bira "prvu napisanu" -- to je greška.
// Ispravno: dodaj specijalizaciju koja pokriva presek, Par<A*, B*>; ona
// je specijalnija od obe i pobeđuje.
template <typename A, typename B>
struct Par {
    static constexpr int v = 0;
};
template <typename A, typename B>
struct Par<A*, B> {
    static constexpr int v = 1;
};
template <typename A, typename B>
struct Par<A, B*> {
    static constexpr int v = 2;
};
int main() { return Par<int*, int*>::v; }
