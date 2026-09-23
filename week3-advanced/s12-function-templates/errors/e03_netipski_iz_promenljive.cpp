// EXPECT-GCC: the value of 'n' is not usable in a constant expression
// EXPECT-CLANG: invalid explicitly-specified argument for template parameter 'N'
// POGREŠNO: ne-tipski argument šablona mora biti poznat pri KOMPAJLIRANJU
// (konstantni izraz). n zavisi od argc -- poznat je tek pri izvršavanju.
// Svaka vrednost N je poseban tip/funkcija koju kompajler mora da napravi.
// Ispravno: običan parametar funkcije (int puta(int x, int n)), ili
// constexpr vrednost kao argument šablona: puta<3>(x).
template <int N>
int puta(int x) { return x * N; }
int main(int argc, char**) {
    const int n = argc;
    return puta<n>(3);
}
