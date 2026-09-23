// EXPECT-UB: execution reached the end of a value-returning function without returning a value
// UB: izlazak sa kraja ne-void funkcije bez return-a ([stmt.return]).
// Kompajler samo upozori (-Wreturn-type, u -Wall), program se kompajlira.
// Ovde su provere napisane pogrešnim redom: za poeni < 50 nijedan return
// se ne izvrši. (main je jedini izuzetak: bez return-a vraća 0.)
// Ispravno: svaka putanja vraća vrednost -- npr. return 5; na kraju
// (main.cpp, sekcija 9).
#include <iostream>
int ocena(int poeni) {
    if (poeni >= 50) return 6;
    if (poeni >= 90) return 10;
}
int main(int argc, char**) {
    std::cout << ocena(19 + argc) << '\n';
}
