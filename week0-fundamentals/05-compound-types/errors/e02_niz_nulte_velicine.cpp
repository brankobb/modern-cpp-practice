// STD: c++17
// EXPECT-GCC: ISO C++ forbids zero-size array
// EXPECT-CLANG: zero size arrays are an extension
// POGREŠNO u standardnom C++-u: niz mora imati bar jedan element.
// (g++ i clang niz nulte veličine puštaju kao ekstenziju bez -pedantic-errors.)
int main() {
    int arr[0];
    return sizeof(arr);
}
