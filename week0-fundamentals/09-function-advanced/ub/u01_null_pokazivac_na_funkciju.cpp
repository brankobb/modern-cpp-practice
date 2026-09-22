// EXPECT-UB: SEGV on unknown address 0x0+ 
// UB: poziv kroz null pokazivač na funkciju -- tipičan "callback koji niko
// nije registrovao".
// Ispravno: if (callback) callback(); -- ili std::function, koji za prazan
// poziv baca std::bad_function_call umesto UB-a.
#include <iostream>
using Callback = void (*)();
Callback onDone = nullptr;
int main() {
    std::cout << "pozivam callback\n";
    onDone();
}
