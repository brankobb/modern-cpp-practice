// EXPECT-UB: stack-buffer-overflow
// UB: std::array::operator[] (kao i C niz) NE proverava granice.
// Ispravno: a.at(i) -- baca std::out_of_range (main.cpp, sekcija 3).
// (Sa -D_GLIBCXX_ASSERTIONS libstdc++ proverava i operator[] u debug build-u.)
#include <array>
#include <iostream>
int main(int argc, char**) {
    std::array<int, 3> a{1, 2, 3};
    int i = argc + 2; // 3 -- jedan iza kraja
    std::cout << a[i] << "\n";
}
