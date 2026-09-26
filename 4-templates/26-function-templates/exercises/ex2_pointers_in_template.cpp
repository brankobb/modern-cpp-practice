// KIND: why
// DEMO-OUT: NAIVE maxOf: banana
//
// Zadatak 2 -- zašto opšti šablon "radi" i kad ne treba (sekcije 1, 5)
// Rešenje: exercises/solutions/ex2_pointers_in_template.cpp
//
// maxOf traži samo operator<. Pokazivači ga imaju -- pa se maxOf za
// const char* kompajlira bez ijednog upozorenja. Ali < na pokazivačima
// poredi ADRESE, ne tekst.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/26-function-templates/exercises/ex2_pointers_in_template.cpp -DNAIVE
//   "cherry" je abecedno posle "banana", a maxOf vrati "banana". Dva niza
//   su članovi iste strukture, pa je b na većoj adresi od a (članovi su u
//   memoriji redom deklaracije) -- maxOf je vratio veću ADRESU.
//   Šablon ne zna šta "veće" znači za tvoj tip; on samo zove operator<.
// Korak 2: ispod šablona dodaj OVERLOAD (ne-šablon)
//   const char* maxOf(const char* a, const char* b) koji poredi sa
//   std::strcmp. Za argumente tipa const char* ne-šablon pobeđuje, jer se
//   tip tačno poklapa (sekcija 5). Proveri da maxOf(3, 7) i dalje ide u šablon.
// Korak 3: (za razmišljanje) zašto overload, a ne eksplicitna
//   specijalizacija template<> const char* maxOf<const char*>(...)? (ex3)

#include <cstring>
#include <iostream>

template <typename T>
T maxOf(T a, T b) {
    return b < a ? a : b;
}

#ifndef NAIVE
// TODO korak 2: overload piši OVDE. Sa -DNAIVE se preskače, pa problem
// možeš da vidiš i posle rešenja.
#endif

struct Pair {
    char a[8] = "cherry";
    char b[8] = "banana";
};

int main() {
    Pair p;
    const char* x = p.a;
    const char* y = p.b;
    std::cout << "maxOf: " << maxOf(x, y) << '\n';
    std::cout << "maxOf(3, 7): " << maxOf(3, 7) << '\n';
}

/* EXPECTED OUTPUT
maxOf: cherry
maxOf(3, 7): 7
*/
