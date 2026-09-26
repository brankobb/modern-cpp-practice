// Rešenje zadatka ex3_specialization_or_overload.

#include <iostream>

template <typename T>
void process(T) {
    std::cout << "generic T\n";
}

template <typename T>
void process(T*) {
    std::cout << "generic T*\n";
}

// Ako za jedan tip pišeš template<> specijalizaciju (nije dobro kad
// postoji više primarnih šablona): ona pripada jednom od njih, zavisno od
// redosleda deklaracija, a poziva se samo ako baš taj šablon pobedi.
// Treba ovako: obična funkcija (overload). Učestvuje u izboru zajedno sa
// šablonima i pobeđuje kad se tip tačno poklapa -- predvidljivo i bez
// obzira na redosled (H. Sutter, "Why Not Specialize Function Templates?").
void process(int*) { std::cout << "special for int*\n"; }

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
