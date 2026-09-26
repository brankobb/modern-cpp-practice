// EXPECT-GCC: passing 'const std::future<int>' as 'this' argument discards qualifiers
// EXPECT-CLANG: 'this' argument to member function 'get' has type 'const std::future<int>', but function is not marked const
// POGREŠNO: future::get() MENJA future -- preuzme rezultat i ostavi ga
// praznog (valid() == false), pa nije const metoda.
// Ispravno: primi future po vrednosti (std::future<int> f, pa pozivalac
// std::move) ili po ne-const referenci; za čitanje bez preuzimanja
// postoji shared_future, čiji je get() const.
#include <future>
int read(const std::future<int>& f) { return f.get(); }
int main() {
    auto f = std::async([] { return 1; });
    return read(f);
}
