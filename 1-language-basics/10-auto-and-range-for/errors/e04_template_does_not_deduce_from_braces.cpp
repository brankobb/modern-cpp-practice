// STD: c++17
// EXPECT-GCC: no matching function for call to 'f(<brace-enclosed initializer list>)'
// EXPECT-CLANG: no matching function for call to 'f'
// POGREŠNO (EMC Item 2): jedina razlika auto i template dedukcije.
// auto x = {1, 2, 3};  -> std::initializer_list<int>  (radi)
// f({1, 2, 3});        -> template NE dedukuje T iz {} (ne radi)
// Ispravno: template <typename T> void f(std::initializer_list<T> list);
template <typename T>
void f(T) {}
int main() {
    f({1, 2, 3});
}
