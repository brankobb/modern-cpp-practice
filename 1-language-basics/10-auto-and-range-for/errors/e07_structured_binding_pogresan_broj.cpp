// STD: c++17
// EXPECT-GCC: only 2 names provided for structured binding
// EXPECT-CLANG: decomposes into 3 elements, but only 2 names were provided
// POGREŠNO: structured binding mora da ima tačno onoliko imena koliko objekat
// ima elemenata.
#include <tuple>
int main() {
    auto [a, b] = std::tuple{1, 2, 3};
    return a + b;
}
