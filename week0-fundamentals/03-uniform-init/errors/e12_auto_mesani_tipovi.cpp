// STD: c++17
// EXPECT-GCC: unable to deduce
// EXPECT-CLANG: deduced conflicting types
// POGREŠNO: auto = {...} pravi std::initializer_list<T>, a svi elementi
// moraju biti ISTOG tipa da bi se T dedukovao (ovde int i double).
#include <initializer_list>
int main() {
    auto w = {1, 2.0};
}
