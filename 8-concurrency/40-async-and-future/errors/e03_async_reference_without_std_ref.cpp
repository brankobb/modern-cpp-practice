// EXPECT-GCC: no matching function for call to 'async(void (&)(int&), int&)'
// EXPECT-CLANG: no matching function for call to 'async'
// POGREŠNO: std::async, kao std::thread (lekcija 39, errors/e01), kopira
// argumente i predaje ih kao rvalue; int& se za rvalue ne veže.
// Ispravno: std::async(increment, std::ref(brojac)) -- i brojac mora da
// živi dok se zadatak ne završi.
#include <future>
void increment(int& x) { ++x; }
int main() {
    int counter = 0;
    auto f = std::async(increment, counter);
    f.get();
    return counter;
}
