// STD: c++17
// EXPECT-GCC: invalid conversion from 'int' to 'int*'
// EXPECT-CLANG: parameter of type 'int *' with an lvalue of type 'int'
// POGREŠNO (EMC Item 8): kad 0 prođe kroz template, dedukuje se kao int,
// i više nije "null pointer constant" -- int se ne može konvertovati u int*.
// Ispravno: call(g, nullptr);  -- std::nullptr_t se konvertuje u int*
template <typename F, typename P>
void call(F func, P param) {
    func(param);
}
void g(int*) {}
int main() {
    call(g, 0);
}
