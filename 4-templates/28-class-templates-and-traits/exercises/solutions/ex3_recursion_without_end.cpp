// Rešenje zadatka ex3_recursion_without_end.

#include <iostream>

// Ako rekurzivni variadic šablon nema kraj (nije dobro): poslednja
// instancijacija poziva funkciju bez argumenata, a nje nema -- greška pri
// kompajliranju.

// a) C++11: osnovni slučaj kao posebna (ne-šablon) funkcija. Mora biti
// deklarisana PRE šablona koji je poziva.
void printA() { std::cout << '\n'; }
template <typename First, typename... Rest>
void printA(const First& p, const Rest&... rest) {
    std::cout << p << (sizeof...(rest) ? " " : "");
    printA(rest...);
}

// b) C++17: if constexpr -- kad nema ostalih, rekurzivni poziv se ne
// instancira uopšte.
template <typename First, typename... Rest>
void printB(const First& p, const Rest&... rest) {
    std::cout << p;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << ' ';
        printB(rest...);
    } else {
        std::cout << '\n';
    }
}

// c) C++17: fold izraz -- bez rekurzije i bez osnovnog slučaja.
template <typename... Args>
void printC(const Args&... args) {
    const char* sep = "";
    ((std::cout << sep << args, sep = " "), ...);
    std::cout << '\n';
}

int main() {
    std::cout << "a: ";
    printA(1, 2.5, "three");
    std::cout << "b: ";
    printB(1, 2.5, "three");
    std::cout << "c: ";
    printC(1, 2.5, "three");
}
