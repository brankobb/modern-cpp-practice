// Rešenje zadatka z2_niz_kao_parametar.

#include <array>
#include <cstddef>
#include <iostream>

// Ako pišeš "int zbir(int niz[10])" (nije dobro): to je "int zbir(int*)",
// veličina se izgubi, a sizeof(niz) meri pokazivač.
// Treba ovako: referenca na niz čuva tip int[N], pa N stiže u funkciju.
template <std::size_t N>
int zbirRef(const int (&niz)[N]) {
    int s = 0;
    for (int x : niz) s += x;      // range-for radi, jer je tip i dalje niz
    return s;
}

// Možeš i ovako: std::array je objekat, veličina je deo tipa i nikad se ne
// raspada u pokazivač.
int zbirArray(const std::array<int, 10>& niz) {
    int s = 0;
    for (int x : niz) s += x;
    return s;
}

int main() {
    int niz[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "zbirRef: " << zbirRef(niz) << '\n';

    std::array<int, 10> a{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::cout << "zbirArray: " << zbirArray(a) << '\n';
}
