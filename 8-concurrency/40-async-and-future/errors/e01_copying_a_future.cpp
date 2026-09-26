// EXPECT-GCC: use of deleted function 'std::future<_Res>::future(const std::future<_Res>&) [with _Res = int]'
// EXPECT-CLANG: call to deleted constructor of 'std::future<int>'
// POGREŠNO: std::future je jedini čitalac rezultata -- get() ga preuzme
// (premesti) i future posle toga nije valid(). Dve kopije bi značile dva
// get()-a istog rezultata, pa je kopiranje obrisano.
// Ispravno: premesti (std::future<int> g = std::move(f);), ili, ako
// result treba više čitalaca: std::shared_future<int> s = f.share();
#include <future>
int main() {
    std::future<int> f = std::async([] { return 1; });
    std::future<int> g = f;
    return g.get();
}
