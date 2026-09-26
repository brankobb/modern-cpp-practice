// Rešenje zadatka ex3_rekurzija_bez_kraja.

#include <iostream>

// Ako rekurzivni variadic šablon nema kraj (nije dobro): poslednja
// instancijacija poziva funkciju bez argumenata, a nje nema -- greška pri
// kompajliranju.

// a) C++11: osnovni slučaj kao posebna (ne-šablon) funkcija. Mora biti
// deklarisana PRE šablona koji je poziva.
void ispisiA() { std::cout << '\n'; }
template <typename Prvi, typename... Ostali>
void ispisiA(const Prvi& p, const Ostali&... ostali) {
    std::cout << p << (sizeof...(ostali) ? " " : "");
    ispisiA(ostali...);
}

// b) C++17: if constexpr -- kad nema ostalih, rekurzivni poziv se ne
// instancira uopšte.
template <typename Prvi, typename... Ostali>
void ispisiB(const Prvi& p, const Ostali&... ostali) {
    std::cout << p;
    if constexpr (sizeof...(ostali) > 0) {
        std::cout << ' ';
        ispisiB(ostali...);
    } else {
        std::cout << '\n';
    }
}

// c) C++17: fold izraz -- bez rekurzije i bez osnovnog slučaja.
template <typename... Args>
void ispisiC(const Args&... args) {
    const char* sep = "";
    ((std::cout << sep << args, sep = " "), ...);
    std::cout << '\n';
}

int main() {
    std::cout << "a: ";
    ispisiA(1, 2.5, "tri");
    std::cout << "b: ";
    ispisiB(1, 2.5, "tri");
    std::cout << "c: ";
    ispisiC(1, 2.5, "tri");
}
