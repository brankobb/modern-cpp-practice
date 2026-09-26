// EXPECT-GCC: may not have reference type 'int&'
// EXPECT-CLANG: static assertion failed due to requirement 'is_object_v<int &>'
// POGREŠNO: optional<T&> ne postoji (do C++26): optional čuva OBJEKAT u
// sebi, a referenca nije objekat. Zato i nema pitanja "šta znači dodela
// optional-u reference" -- menja li referencu ili ono na šta pokazuje.
// Ispravno: pokazivač (int* -- nullptr je "nema"), ili
// std::optional<std::reference_wrapper<int>>.
#include <optional>
int main() {
    int x = 5;
    std::optional<int&> r = x;
    return *r;
}
