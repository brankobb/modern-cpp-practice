// EXPECT-RUN: terminate called after throwing an instance of 'std::bad_function_call'
// NIJE UB, ali program se prekine: poziv PRAZNOG std::function-a baca
// std::bad_function_call ([func.wrap.func.inv]); niko ga ne hvata, pa
// std::terminate (lekcija 18, runtime/r01).
// Ispravno: proveri pre poziva -- if (onClick) onClick(); -- ili dodeli
// podrazumevani callback koji ne radi ništa: onClick = [] {};
#include <functional>
#include <iostream>
struct Button {
    std::function<void()> onClick;
    void click() { onClick(); }
};
int main() {
    Button d;
    std::cout << "click\n";
    d.click();
}
