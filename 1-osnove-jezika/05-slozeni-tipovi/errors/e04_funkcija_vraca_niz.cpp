// STD: c++17
// EXPECT-GCC: declared as function returning an array
// EXPECT-CLANG: function cannot return array type
// POGREŠNO: funkcija ne može da vrati C niz.
// Ispravno: std::array<int, 3> makeArray();
int makeArray()[3];
int main() {}
