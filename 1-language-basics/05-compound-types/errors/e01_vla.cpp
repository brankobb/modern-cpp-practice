// STD: c++17
// EXPECT-GCC: ISO C++ forbids variable length array
// EXPECT-CLANG: variable length arrays in C++ are a Clang extension
// POGREŠNO u C++-u: veličina C niza mora biti konstantni izraz.
// "Variable length array" je C99. g++ ga bez -pedantic-errors pušta kao
// ekstenziju, a clang ga i SA -pedantic-errors samo upozori -- zato build
// koristi i -Werror=vla.
// Ispravno: std::vector<int> arr(n);
int main(int argc, char**) {
    int n = argc + 4;
    int arr[n];
    arr[0] = 1;
    return arr[0];
}
