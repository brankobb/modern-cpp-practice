// EXPECT-GCC: parameter packs not expanded with '...'
// EXPECT-CLANG: initializer contains unexpanded parameter pack 'args'
// POGREŠNO: args je PAKET vrednosti, ne jedna vrednost. Svaka upotreba
// mora da ga "proširi" sa ...: args... (svi redom), f(args)... (obrazac
// za svaki), ili fold izraz.
// Ispravno: int arr[] = {args...};
template <typename... Args>
int first(Args... args) {
    int arr[] = {args};
    return arr[0];
}
int main() { return first(1, 2, 3); }
