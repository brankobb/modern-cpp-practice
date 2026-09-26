// EXPECT-GCC: need 'typename' before 'std::remove_reference<
// EXPECT-CLANG: missing 'typename' prior to dependent type name 'std::remove_reference<T>::type'
// POGREŠNO: std::remove_reference<T>::type zavisi od T; pre instancijacije
// kompajler ne zna da li je ::type tip ili vrednost, i podrazumeva
// vrednost. (g++ u poruci ispisuje čudno ime parametra, _Functor.)
// Ispravno: typename std::remove_reference<T>::type kopija{};
// ili kraće, bez typename: std::remove_reference_t<T> kopija{};  (C++14 _t)
#include <type_traits>
template <typename T>
void f(T&&) {
    std::remove_reference<T>::type kopija{};
    (void)kopija;
}
int main() {
    int x = 0;
    f(x);
}
