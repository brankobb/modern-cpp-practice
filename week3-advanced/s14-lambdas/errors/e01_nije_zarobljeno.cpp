// EXPECT-GCC: 'prag' is not captured
// EXPECT-CLANG: variable 'prag' cannot be implicitly captured in a lambda with no capture-default specified
// POGREŠNO: lambda sa praznim [] ne vidi lokalne promenljive okolne
// funkcije. Lambda je klasa (sekcija 4) -- lokalna promenljiva mora
// eksplicitno da postane njen član (capture).
// Ispravno: [prag] (kopija) ili [&prag] (referenca). [=] / [&] rade, ali
// sakriju šta se sve zarobljava.
#include <algorithm>
#include <vector>
int main() {
    std::vector<int> v{1, 5, 9};
    int prag = 4;
    return static_cast<int>(std::count_if(v.begin(), v.end(), [](int x) { return x > prag; }));
}
