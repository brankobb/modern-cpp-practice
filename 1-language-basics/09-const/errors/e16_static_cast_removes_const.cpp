// STD: c++17
// EXPECT-GCC: invalid 'static_cast' from type 'const int*' to type 'int*'
// EXPECT-CLANG: static_cast from 'const int *' to 'int *' is not allowed
// POGREŠNO: static_cast (i ostali cast-ovi osim const_cast) ne sme da skine
// const. Jedini cast koji skida const je const_cast -- namerno, da se vidi.
int main() {
    int x = 1;
    const int* cp = &x;
    int* p = static_cast<int*>(cp);
    return *p;
}
