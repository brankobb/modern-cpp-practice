// STD: c++17
// EXPECT-GCC: cannot declare reference to 'int&'
// EXPECT-CLANG: declared as a reference to a reference
// POGREŠNO: referenca na referencu se ne može napisati direktno.
// Kroz alias/template postoji "reference collapsing": using R = int&; R& rr = x; -> int&
int main() {
    int x = 1;
    int& & rr = x;
    return rr;
}
