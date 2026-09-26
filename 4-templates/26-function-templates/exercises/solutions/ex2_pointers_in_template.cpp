// Rešenje zadatka ex2_pointers_in_template.

#include <cstring>
#include <iostream>

template <typename T>
T maxOf(T a, T b) {
    return b < a ? a : b;
}

// Ako se osloniš na opšti šablon za const char* (nije dobro): < poredi
// adrese, pa rezultat zavisi od rasporeda u memoriji, ne od teksta.
// Treba ovako: overload za tip kome treba drugačije značenje. Ne-šablon
// sa tačnim tipom pobeđuje šablon u overload resolution-u.
const char* maxOf(const char* a, const char* b) { return std::strcmp(b, a) < 0 ? a : b; }

// Korak 3: specijalizacija ne učestvuje u overload resolution-u -- bira
// se tek POSLE izbora primarnog šablona, pa u prisustvu drugih overload-a
// lako ispadne da se ne pozove (zadatak ex3). Overload je uvek vidljiv.

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
