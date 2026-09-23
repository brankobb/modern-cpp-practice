// EXPECT-GCC: parameter packs not expanded with '...'
// EXPECT-CLANG: expression contains unexpanded parameter pack 'args'
// POGREŠNO: zagrade su DEO sintakse fold izraza: ( paket op ... ).
// Bez njih kompajler vidi "args +" i nerazvijen paket.
// Ispravno: return (args + ...);
template <typename... T>
int zbir(T... args) {
    return args + ...;
}
int main() { return zbir(1, 2); }
