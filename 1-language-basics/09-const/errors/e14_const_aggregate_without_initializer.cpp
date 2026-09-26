// STD: c++17
// EXPECT-GCC: uninitialized 'const a'
// EXPECT-CLANG: without a user-provided default constructor
// POGREŠNO: const objekat tipa bez korisničkog default konstruktora i bez
// default member initializer-a bi imao neodređene članove zauvek.
// Ispravno: const A a{};  (value-init -> x == 0)
// (const std::string s; JE ispravno -- string ima korisnički default ctor.)
struct A {
    int x;
};
int main() {
    const A a;
    return a.x;
}
