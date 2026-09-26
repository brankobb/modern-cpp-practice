// STD: c++17
// EXPECT-GCC: forming pointer to reference type 'int&'
// EXPECT-CLANG: declared as a pointer to a reference of type 'int &'
// POGREŠNO: std::vector<int&>.
// Zašto: element kontejnera mora biti objekat koji se može kopirati/dodeliti
//   i na koji može da se napravi pokazivač. Referenca nije objekat (nema
//   sopstvenu adresu, ne može se "preusmeriti"), pa T* za T = int& ne postoji.
// Ispravno: std::vector<int*> (posmatrači), ili
//   std::vector<std::reference_wrapper<int>> (ponaša se kao referenca, a
//   jeste objekat).
#include <vector>

int main() {
    int a = 1;
    std::vector<int&> refs{a};
    return refs[0];
}
