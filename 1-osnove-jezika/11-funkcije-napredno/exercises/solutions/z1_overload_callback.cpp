// Rešenje zadatka z1_overload_callback.

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Korak 1: 'a' je char -> promocija u int (bolje od konverzije u double ili
// bool), 1.5f je float -> promocija u double. "x" je const char[2] ->
// const char* (array-to-pointer je "exact match" kategorija).
void opisi(int x) { std::cout << "int " << x << '\n'; }
void opisi(double x) { std::cout << "double " << x << '\n'; }
void opisi(const char* x) { std::cout << "const char* " << x << '\n'; }
void opisi(bool x) { std::cout << "bool " << x << '\n'; }

// Korak 2: podrazumevani argumenti idu u deklaraciju (u header-u, da ih
// vide svi pozivaoci), i samo zdesna nalevo.
std::string formatiraj(double v, int decimale = 2, char sep = '.');

std::string formatiraj(double v, int decimale, char sep) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(decimale) << v;
    std::string s = os.str();
    for (char& c : s)
        if (c == '.') c = sep;
    return s;
}

// Korak 3: pokazivač na funkciju kao callback (C stil, i dalje čest u
// embedded kodu). nullptr znači "nema handlera" -- proveri pre poziva
// (poziv nullptr pokazivača je UB, ub/u01).
struct Dugme {
    void (*naKlik)(int) = nullptr;
    void klik(int x) const {
        if (naKlik)
            naKlik(x);
        else
            std::cout << "dugme bez handlera\n";
    }
};

void prijavi(int x) { std::cout << "klik: " << x << '\n'; }

int main() {
    opisi(42);
    opisi('a');
    opisi(1.5f);
    opisi("x");
    opisi(true);

    std::cout << formatiraj(3.14159) << ' ' << formatiraj(3.14159, 3) << ' '
              << formatiraj(3.14159, 2, ',') << '\n';

    Dugme d;
    d.klik(1);
    d.naKlik = prijavi;
    d.klik(5);
    // Lambda bez hvatanja ima implicitnu konverziju u pokazivač na funkciju.
    // Sa hvatanjem ([&]) ne bi mogla (errors/e09) -- tada std::function.
    d.naKlik = [](int x) { std::cout << "lambda: " << x * 10 << '\n'; };
    d.klik(5);
}
