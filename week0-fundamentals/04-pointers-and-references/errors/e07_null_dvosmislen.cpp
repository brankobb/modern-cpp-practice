// STD: c++17
// EXPECT-GCC: call of overloaded 'f(NULL)' is ambiguous
// EXPECT-CLANG: call to 'f' is ambiguous
// POGREŠNO (EMC Item 8): NULL je celobrojna konstanta, ne pokazivač.
// Sa overload-ima f(int) i f(char*) poziv f(NULL) je dvosmislen.
// Ispravno: f(nullptr) -> f(char*);  f(0) -> f(int)
#include <cstddef>
void f(int) {}
void f(char*) {}
int main() {
    f(NULL);
}
