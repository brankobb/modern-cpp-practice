// STD: c++17
// EXPECT-GCC: uninitialized 'const x'
// EXPECT-CLANG: default initialization of an object of const type 'const int'
// POGREŠNO: const objekat mora da dobije vrednost pri definiciji -- posle toga
// se više ne može menjati, pa bi ostao zauvek neodređen.
// Ispravno: const int x = 42;  (vrednost sme da bude i izračunata u runtime-u)
int main() {
    const int x;
    return x;
}
