// EXPECT-RUN: terminate called after throwing an instance of 'std::bad_function_call'
// NIJE UB, ali program se prekine: poziv PRAZNOG std::function-a baca
// std::bad_function_call ([func.wrap.func.inv]); niko ga ne hvata, pa
// std::terminate (lekcija 18, runtime/r01).
// Ispravno: proveri pre poziva -- if (naKlik) naKlik(); -- ili dodeli
// podrazumevani callback koji ne radi ništa: naKlik = [] {};
#include <functional>
#include <iostream>
struct Dugme {
    std::function<void()> naKlik;
    void klik() { naKlik(); }
};
int main() {
    Dugme d;
    std::cout << "klik\n";
    d.klik();
}
