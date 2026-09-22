// STD: c++17
// EXPECT-GCC: pointer of type 'void *' used in arithmetic
// EXPECT-CLANG: arithmetic on a pointer to void
// POGREŠNO: aritmetika nad void* nije dozvoljena (void nema veličinu).
// g++ je bez -pedantic-errors pušta kao GNU ekstenziju (korak = 1 bajt); clang je u C++-u uvek odbija.
int main() {
    int x = 1;
    void* vp = &x;
    void* next = vp + 1;
    return next != nullptr;
}
