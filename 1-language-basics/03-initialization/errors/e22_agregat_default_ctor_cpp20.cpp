// STD: c++20
// EXPECT-GCC: no matching function for call to 'S::S(<brace-enclosed
// EXPECT-CLANG: no matching constructor for initialization of 'S'
// POGREŠNO u C++20, ISPRAVNO u C++17: struktura sa "S() = default;" je
// agregat u C++17 (pravilo: nema user-PROVIDED ctor-a), ali NIJE u C++20
// (pravilo: nema user-DECLARED ctor-a). Poznata breaking promena.
struct S {
    S() = default;
    int x;
    int y;
};
int main() {
    S s{1, 2};
    (void)s;
}
