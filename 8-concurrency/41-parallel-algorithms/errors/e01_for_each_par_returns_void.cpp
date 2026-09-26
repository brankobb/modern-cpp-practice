// EXPECT-GCC: deduced type 'void' for 'b' is incomplete
// EXPECT-CLANG: variable has incomplete type
// POGREŠNO: sekvencijalni for_each vraća funkcijski objekat (sa stanjem
// koje je nakupio). Paralelni vraća void: svaka nit radi sa svojom
// KOPIJOM funktora, pa ne postoji jedno "nakupljeno" stanje koje bi
// mogao da vrati.
// Ispravno: algoritam koji sam skuplja rezultat --
//   auto n = std::count_if(std::execution::par, v.begin(), v.end(), [](int x) { return x > 0; });
#include <algorithm>
#include <execution>
#include <vector>
struct Counter {
    int n = 0;
    void operator()(int x) {
        if (x > 0) ++n;
    }
};
int main() {
    std::vector<int> v{1, -2, 3};
    auto b = std::for_each(std::execution::par, v.begin(), v.end(), Counter{});
    return b.n;
}
