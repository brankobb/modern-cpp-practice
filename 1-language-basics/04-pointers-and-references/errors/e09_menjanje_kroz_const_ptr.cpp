// STD: c++17
// EXPECT-GCC: assignment of read-only location
// EXPECT-CLANG: read-only variable is not assignable
// POGREŠNO: const int* -- vrednost na koju pokazuje ne sme da se menja KROZ p.
// (Sam pokazivač sme da se preusmeri: p = &y; je OK.)
int main() {
    int x = 1;
    const int* p = &x;
    *p = 5;
    return x;
}
