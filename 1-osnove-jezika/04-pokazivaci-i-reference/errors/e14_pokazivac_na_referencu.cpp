// STD: c++17
// EXPECT-GCC: cannot declare pointer to 'int&'
// EXPECT-CLANG: declared as a pointer to a reference
// POGREŠNO: pokazivač na referencu ne postoji -- referenca nije objekat i
// nema sopstvenu adresu. (&r daje adresu objekta na koji r referiše.)
// Ispravno: referenca na pokazivač postoji: int*& rp = p;
int main() {
    int x = 1;
    int& r = x;
    int&* p = &r;
    return *p;
}
