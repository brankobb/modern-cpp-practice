// STD: c++17
// EXPECT-GCC: must be a non-static member function
// EXPECT-CLANG: overloaded 'operator=' must be a non-static member function
// POGREŠNO: operator= kao slobodna funkcija.
// Zašto: =, (), [] i -> moraju biti članovi klase ([over.ass], [over.call],
//   [over.sub], [over.ref]). Za = je razlog i to što kompajler sam pravi
//   operator= za svaku klasu; slobodna verzija bi se sa njim sudarala.
// Ispravno: Vec& operator=(const Vec& rhs) unutar klase, ili ga ne piši
//   uopšte ako je kompajlerov dovoljno dobar (rule of 0).
struct Vec {
    double x;
};

Vec& operator=(Vec& lhs, const Vec& rhs) {
    lhs.x = rhs.x;
    return lhs;
}

int main() {}
