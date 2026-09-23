// EXPECT-GCC: template instantiation depth exceeds maximum of 900
// EXPECT-CLANG: recursive template instantiation exceeded maximum depth of 1024
// POGREŠNO: sa običnim if, OBE grane se prevode za svaki N. Grana
// N * fakt<N - 1>() se prevodi i za N = 1, pa se traži fakt<0>, fakt<-1>
// ... bez kraja -- iako se pri izvršavanju nikad ne bi pozvala.
// Ispravno: if constexpr (N <= 1) -- za N <= 1 druga grana se ne
// instancira (main.cpp, sekcija 6).
template <int N>
constexpr int fakt() {
    if (N <= 1)
        return 1;
    else
        return N * fakt<N - 1>();
}
int main() { return fakt<5>(); }
