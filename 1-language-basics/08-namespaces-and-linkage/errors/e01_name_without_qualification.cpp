// STD: c++17
// EXPECT-GCC: 'cout' was not declared in this scope
// EXPECT-CLANG: use of undeclared identifier 'cout'
// POGREŠNO: ime iz namespace-a korišćeno bez kvalifikacije.
// Zašto: cout je deklarisan u namespace-u std. Obično (nekvalifikovano)
//   traženje imena gleda trenutni scope i okolne, a std nije među njima.
//   ADL ovde ne pomaže, jer cout nije poziv funkcije sa argumentom iz std.
// Ispravno: std::cout, ili "using std::cout;" u funkciji ili .cpp fajlu.
#include <iostream>

int main() {
    cout << "hello\n";
}
