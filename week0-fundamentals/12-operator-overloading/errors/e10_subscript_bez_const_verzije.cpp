// STD: c++17
// EXPECT-GCC: passing 'const Matrix' as 'this' argument discards qualifiers
// EXPECT-CLANG: no viable overloaded operator[] for type 'const Matrix'
// POGREŠNO: samo ne-const operator[], a objekat se čita kroz const&.
// Zašto: na const objektu se smeju zvati samo const funkcije (lekcija 07).
//   Kako se objekti najčešće prosleđuju kao const T&, klasa bez const
//   operator[] se praktično ne može čitati.
// Ispravno: dve verzije (main.cpp, sekcija 7):
//   int& operator[](int i);  const int& operator[](int i) const;
struct Matrix {
    int data[4] {};
    int& operator[](int i) { return data[i]; }
};

int read(const Matrix& m) { return m[0]; }

int main() {
    Matrix m;
    return read(m);
}
