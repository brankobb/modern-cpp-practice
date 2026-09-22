#include <iostream>

void print(int x) { std::cout << "print(int): " << x << "\n"; }
void print(double x) { std::cout << "print(double): " << x << "\n"; }

void greet(std::string name, int times = 1) {
    for (int i = 0; i < times; ++i) std::cout << "Hi " << name << "\n";
}
// Ako dodaš void greet(std::string name); pored greet(name, times=1) (NIJE
// DOBRO) jer greet("x") postaje DVOSMISLEN poziv -- kompajler ne zna da li
// da pozove overload bez drugog parametra ili overload sa default
// vrednošću -- COMPILE ERROR, ne runtime bag.
// Treba da izbegavaš overload čiji potpis "preklapa" default argument
// nekog drugog overload-a.
// Možeš i umesto overload-a koristiti JEDNU funkciju sa default
// argumentom (kao ovde) -- jednostavnije, bez rizika od ambiguity-ja.
// void greet(std::string name); // TODO: otkomentariši -- ambiguity greška

// Ako NE staviš inline na funkciju definisanu u header fajlu koji se
// uključuje u više .cpp fajlova (NIJE DOBRO) jer svaki .cpp dobija SVOJU
// definiciju iste funkcije -- linker javlja "multiple definition"
// (kršenje One Definition Rule).
// Treba da staviš inline (ovde smo u jednom main.cpp pa se problem ne
// vidi, ali princip važi za header fajlove).
// Možeš i definisati funkciju u .cpp fajlu a samo DEKLARISATI u
// header-u -- klasičan pristup bez inline, samo malo više fajlova.
inline int square(int x) { return x * x; } // bezbedno u header-u, bez ODR problema

using BinaryOp = int (*)(int, int); // čitljiviji nego "int (*)(int, int)" inline
int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

namespace mymath {
    struct Vec2 { double x, y; };
    double length(const Vec2& v) { return v.x + v.y; } // pojednostavljeno
}

int main() {
    std::cout << "-- overload resolution --\n";
    print(5);     // print(int)
    print(5.0);   // print(double)

    std::cout << "-- default argument --\n";
    greet("Ana", 2);

    std::cout << "-- inline --\n";
    std::cout << square(6) << "\n";

    std::cout << "-- function pointer --\n";
    BinaryOp op = add;
    std::cout << "op(2,3)=" << op(2, 3) << "\n";
    op = mul;
    std::cout << "op(2,3)=" << op(2, 3) << "\n";

    std::cout << "-- namespace + ADL --\n";
    mymath::Vec2 v{3, 4};
    // Ako uvek pišeš using namespace mymath; na global scope-u (NIJE
    // DOBRO u header fajlu) jer zagađuje SVAKI fajl koji ga uključi -- svi
    // identifikatori iz mymath postaju vidljivi svuda, veći rizik od name
    // collision-a.
    // Treba da koristiš eksplicitan prefiks (mymath::length(v)) ili
    // ciljanu deklaraciju (using mymath::length;) kad ti treba samo JEDNO
    // ime, ne ceo namespace.
    // Možeš i osloniti se na ADL (argument-dependent lookup) -- length(v)
    // bi radio i BEZ prefiksa jer je v tipa mymath::Vec2, kompajler
    // automatski traži i u tom namespace-u.
    std::cout << "length=" << mymath::length(v) << "\n";
}
