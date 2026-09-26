// Rešenje zadatka ex1_parametri.

#include <cstddef>
#include <iostream>

struct Senzor {
    const char* ime;
    double temp;
};

// Korak 1: referenca kad argument MORA da postoji, pokazivač kad sme da ga
// nema (nullptr) -- tada je provera deo ugovora funkcije.
void zameni(int& a, int& b) {
    int t = a;
    a = b;
    b = t;
}

bool zameniPtr(int* a, int* b) {
    if (a == nullptr || b == nullptr) return false;
    zameni(*a, *b);
    return true;
}

// Korak 2: [first, last) -- last pokazuje JEDAN IZA kraja (sme da se
// napravi i poredi, ne sme da se dereferencira). const int*: funkcija
// obećava da ne menja elemente, pa prima i const nizove.
int zbir(const int* first, const int* last) {
    int s = 0;
    for (const int* p = first; p != last; ++p) s += *p;
    return s;
}

// Korak 3: rezultat možda ne postoji -> pokazivač (nullptr = "nema").
// Referenca bi morala da se veže za nešto i kad je niz prazan.
const Senzor* najtopliji(const Senzor* niz, std::size_t n) {
    if (n == 0) return nullptr;
    const Senzor* best = niz;
    for (const Senzor* p = niz + 1; p != niz + n; ++p)
        if (p->temp > best->temp) best = p;
    return best;
}

int main() {
    int x = 1, y = 2;
    zameni(x, y);
    std::cout << "zameni: x=" << x << " y=" << y << '\n';
    bool ok = zameniPtr(&x, &y);
    std::cout << "zameniPtr: " << ok << " x=" << x << " y=" << y << '\n';
    std::cout << "zameniPtr(nullptr): " << zameniPtr(&x, nullptr) << '\n';

    int niz[] = {1, 2, 3, 4, 5};
    std::cout << "zbir svih: " << zbir(niz, niz + 5) << '\n';
    std::cout << "zbir [1, 3): " << zbir(niz + 1, niz + 3) << '\n';
    std::cout << "zbir praznog: " << zbir(niz, niz) << '\n';

    Senzor s[] = {{"motor", 71.5}, {"baterija", 38.0}, {"cpu", 84.25}};
    if (const Senzor* p = najtopliji(s, 3))
        std::cout << "najtopliji: " << p->ime << ' ' << p->temp << '\n';
    if (najtopliji(s, 0) == nullptr)
        std::cout << "prazan niz: nema rezultata\n";
}
