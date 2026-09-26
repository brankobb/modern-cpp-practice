// STD: c++14
// EXPECT-GCC: use of deleted function
// EXPECT-CLANG: invokes deleted constructor
// POGREŠNO u C++14, ISPRAVNO od C++17: std::atomic nije kopirljiv, a u C++14
// "= 0" formalno traži copy/move konstruktor. C++17 garantuje copy elision,
// pa se objekat pravi direktno i ovo radi. (EMC Item 7 je pisan za C++14.)
#include <atomic>
int main() {
    std::atomic<int> a = 0;
    return a.load();
}
