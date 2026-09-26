// STD: c++17
// EXPECT-GCC: redefinition of 'void f(int)'
// EXPECT-CLANG: redefinition of 'f'
// POGREŠNO: const na parametru PO VREDNOSTI (top-level const) nije deo
// potpisa funkcije -- f(int) i f(const int) su ista funkcija, pa je ovo
// dvostruka definicija. (Za pokazivače/reference, "low-level" const JESTE
// deo potpisa: f(int*) i f(const int*) su dve različite funkcije.)
void f(int) {}
void f(const int) {}
int main() {
    f(1);
}
