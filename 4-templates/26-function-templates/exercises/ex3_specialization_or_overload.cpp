// KIND: why
// DEMO-OUT: NAIVE call with int\*: generic T\*
//
// Zadatak 3 -- zašto overload umesto eksplicitne specijalizacije
// funkcijskog šablona (sekcija 5)
// Rešenje: exercises/solutions/ex3_specialization_or_overload.cpp
//
// Postoje dva šablona: process(T) i process(T*). Za int* hoćemo posebno
// ponašanje, pa je neko napisao template<> specijalizaciju.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/26-function-templates/exercises/ex3_specialization_or_overload.cpp -DNAIVE
//   Specijalizacija se NE pozove. Zašto:
//   a) template<> void process<>(int*) specijalizuje šablon koji je u tom
//      trenutku VIDLJIV -- ovde samo process(T), sa T = int*;
//   b) poziv process(p) prvo bira između PRIMARNIH šablona: process(T) i
//      process(T*). process(T*) je specijalniji i pobeđuje;
//   c) specijalizacije se gledaju tek posle toga, i samo za pobednika --
//      a naša pripada gubitniku.
//   Premesti specijalizaciju ispod process(T*) i ponašanje se promeni
//   (proveri!) -- redosled deklaracija menja koja funkcija se zove.
// Korak 2: u #else grani umesto specijalizacije napiši OVERLOAD:
//   void process(int*) (ne-šablon). Ne-šablon sa tačnim tipom učestvuje u
//   izboru ravnopravno, i pobeđuje na izjednačenju -- bez obzira na
//   redosled.

#include <iostream>

template <typename T>
void process(T) {
    std::cout << "generic T\n";
}

#ifdef NAIVE
template <>
void process<>(int*) {
    std::cout << "specialization for int*\n";
}
#endif

template <typename T>
void process(T*) {
    std::cout << "generic T*\n";
}

// TODO korak 2

int main() {
    int x = 1;
    double d = 2.0;
    std::cout << "call with int*: ";
    process(&x);
    std::cout << "call with double*: ";
    process(&d);
    std::cout << "call with int: ";
    process(x);
}

/* EXPECTED OUTPUT
call with int*: special for int*
call with double*: generic T*
call with int: generic T
*/
