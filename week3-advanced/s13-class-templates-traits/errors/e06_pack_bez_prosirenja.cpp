// EXPECT-GCC: parameter packs not expanded with '...'
// EXPECT-CLANG: initializer contains unexpanded parameter pack 'args'
// POGREŠNO: args je PAKET vrednosti, ne jedna vrednost. Svaka upotreba
// mora da ga "proširi" sa ...: args... (svi redom), f(args)... (obrazac
// za svaki), ili fold izraz.
// Ispravno: int niz[] = {args...};
template <typename... Args>
int prvi(Args... args) {
    int niz[] = {args};
    return niz[0];
}
int main() { return prvi(1, 2, 3); }
