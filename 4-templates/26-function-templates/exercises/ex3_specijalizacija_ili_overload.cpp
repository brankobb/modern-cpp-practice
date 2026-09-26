// KIND: why
// DEMO-OUT: NAIVNO poziv sa int\*: opšti T\*
//
// Zadatak 3 -- zašto overload umesto eksplicitne specijalizacije
// funkcijskog šablona (sekcija 5)
// Rešenje: exercises/solutions/ex3_specijalizacija_ili_overload.cpp
//
// Postoje dva šablona: obradi(T) i obradi(T*). Za int* hoćemo posebno
// ponašanje, pa je neko napisao template<> specijalizaciju.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/26-function-templates/exercises/ex3_specijalizacija_ili_overload.cpp -DNAIVNO
//   Specijalizacija se NE pozove. Zašto:
//   a) template<> void obradi<>(int*) specijalizuje šablon koji je u tom
//      trenutku VIDLJIV -- ovde samo obradi(T), sa T = int*;
//   b) poziv obradi(p) prvo bira između PRIMARNIH šablona: obradi(T) i
//      obradi(T*). obradi(T*) je specijalniji i pobeđuje;
//   c) specijalizacije se gledaju tek posle toga, i samo za pobednika --
//      a naša pripada gubitniku.
//   Premesti specijalizaciju ispod obradi(T*) i ponašanje se promeni
//   (proveri!) -- redosled deklaracija menja koja funkcija se zove.
// Korak 2: u #else grani umesto specijalizacije napiši OVERLOAD:
//   void obradi(int*) (ne-šablon). Ne-šablon sa tačnim tipom učestvuje u
//   izboru ravnopravno, i pobeđuje na izjednačenju -- bez obzira na
//   redosled.

#include <iostream>

template <typename T>
void obradi(T) {
    std::cout << "opšti T\n";
}

#ifdef NAIVNO
template <>
void obradi<>(int*) {
    std::cout << "specijalizacija za int*\n";
}
#endif

template <typename T>
void obradi(T*) {
    std::cout << "opšti T*\n";
}

// TODO korak 2

int main() {
    int x = 1;
    double d = 2.0;
    std::cout << "poziv sa int*: ";
    obradi(&x);
    std::cout << "poziv sa double*: ";
    obradi(&d);
    std::cout << "poziv sa int: ";
    obradi(x);
}

/* EXPECTED OUTPUT
poziv sa int*: posebno za int*
poziv sa double*: opšti T*
poziv sa int: opšti T
*/
