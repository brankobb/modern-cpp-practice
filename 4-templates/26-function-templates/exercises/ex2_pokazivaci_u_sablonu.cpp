// KIND: why
// DEMO-OUT: NAIVE maks: banana
//
// Zadatak 2 -- zašto opšti šablon "radi" i kad ne treba (sekcije 1, 5)
// Rešenje: exercises/solutions/ex2_pokazivaci_u_sablonu.cpp
//
// maks traži samo operator<. Pokazivači ga imaju -- pa se maks za
// const char* kompajlira bez ijednog upozorenja. Ali < na pokazivačima
// poredi ADRESE, ne tekst.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/26-function-templates/exercises/ex2_pokazivaci_u_sablonu.cpp -DNAIVE
//   "jabuka" je abecedno posle "banana", a maks vrati "banana". Dva niza
//   su članovi iste strukture, pa je b na većoj adresi od a (članovi su u
//   memoriji redom deklaracije) -- maks je vratio veću ADRESU.
//   Šablon ne zna šta "veće" znači za tvoj tip; on samo zove operator<.
// Korak 2: ispod šablona dodaj OVERLOAD (ne-šablon)
//   const char* maks(const char* a, const char* b) koji poredi sa
//   std::strcmp. Za argumente tipa const char* ne-šablon pobeđuje, jer se
//   tip tačno poklapa (sekcija 5). Proveri da maks(3, 7) i dalje ide u šablon.
// Korak 3: (za razmišljanje) zašto overload, a ne eksplicitna
//   specijalizacija template<> const char* maks<const char*>(...)? (ex3)

#include <cstring>
#include <iostream>

template <typename T>
T maks(T a, T b) {
    return b < a ? a : b;
}

#ifndef NAIVE
// TODO korak 2: overload piši OVDE. Sa -DNAIVE se preskače, pa problem
// možeš da vidiš i posle rešenja.
#endif

struct Par {
    char a[8] = "jabuka";
    char b[8] = "banana";
};

int main() {
    Par p;
    const char* x = p.a;
    const char* y = p.b;
    std::cout << "maks: " << maks(x, y) << '\n';
    std::cout << "maks(3, 7): " << maks(3, 7) << '\n';
}

/* EXPECTED OUTPUT
maks: jabuka
maks(3, 7): 7
*/
