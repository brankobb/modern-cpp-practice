// EXPECT-UB: null pointer|SEGV|stack-use-after-return
// UB (EC++ Item 21): vraćanje adrese lokalne promenljive -- lokalna nestaje
// čim funkcija vrati. Kompajler upozori (-Wreturn-local-addr); tretiraj to
// upozorenje kao grešku. Zanimljivo: g++ ovde namerno vraća nullptr (kod je
// ionako UB), pa se program sruši na null dereferenciranju.
// Ispravno: vrati po vrednosti -- int make() { int local = 42; return local; }
#include <iostream>
int* make() {
    int local = 42;
    return &local;
}
int main() {
    int* p = make();
    std::cout << *p << "\n";
}
