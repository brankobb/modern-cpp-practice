// EXPECT-GCC: no matching function for call to 'std::promise<int>::set_value()'
// EXPECT-CLANG: no matching member function for call to 'set_value'
// POGREŠNO: promise<int> obećava int; set_value() bez argumenta postoji
// samo za promise<void> (signal "gotovo", bez vrednosti).
// Ispravno: p.set_value(42); ili std::promise<void> ako vrednost ne treba.
#include <future>
int main() {
    std::promise<int> p;
    std::future<int> f = p.get_future();
    p.set_value();
    return f.get();
}
