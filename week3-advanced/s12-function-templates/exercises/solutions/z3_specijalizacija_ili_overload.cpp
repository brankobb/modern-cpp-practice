// Rešenje zadatka z3_specijalizacija_ili_overload.

#include <iostream>

template <typename T>
void obradi(T) {
    std::cout << "opšti T\n";
}

template <typename T>
void obradi(T*) {
    std::cout << "opšti T*\n";
}

// Ako za jedan tip pišeš template<> specijalizaciju (nije dobro kad
// postoji više primarnih šablona): ona pripada jednom od njih, zavisno od
// redosleda deklaracija, a poziva se samo ako baš taj šablon pobedi.
// Treba ovako: obična funkcija (overload). Učestvuje u izboru zajedno sa
// šablonima i pobeđuje kad se tip tačno poklapa -- predvidljivo i bez
// obzira na redosled (H. Sutter, "Why Not Specialize Function Templates?").
void obradi(int*) { std::cout << "posebno za int*\n"; }

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
