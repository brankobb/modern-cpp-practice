// Rešenje zadatka ex1_overload_callback.

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// Korak 1: 'a' je char -> promocija u int (bolje od konverzije u double ili
// bool), 1.5f je float -> promocija u double. "x" je const char[2] ->
// const char* (array-to-pointer je "exact match" kategorija).
void describe(int x) { std::cout << "int " << x << '\n'; }
void describe(double x) { std::cout << "double " << x << '\n'; }
void describe(const char* x) { std::cout << "const char* " << x << '\n'; }
void describe(bool x) { std::cout << "bool " << x << '\n'; }

// Korak 2: podrazumevani argumenti idu u deklaraciju (u header-u, da ih
// vide svi pozivaoci), i samo zdesna nalevo.
std::string formatNumber(double v, int decimals = 2, char sep = '.');

std::string formatNumber(double v, int decimals, char sep) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(decimals) << v;
    std::string s = os.str();
    for (char& c : s)
        if (c == '.') c = sep;
    return s;
}

// Korak 3: pokazivač na funkciju kao callback (C stil, i dalje čest u
// embedded kodu). nullptr znači "nema handlera" -- proveri pre poziva
// (poziv nullptr pokazivača je UB, ub/u01).
struct Button {
    void (*onClick)(int) = nullptr;
    void click(int x) const {
        if (onClick)
            onClick(x);
        else
            std::cout << "button without a handler\n";
    }
};

void report(int x) { std::cout << "click: " << x << '\n'; }

int main() {
    describe(42);
    describe('a');
    describe(1.5f);
    describe("x");
    describe(true);

    std::cout << formatNumber(3.14159) << ' ' << formatNumber(3.14159, 3) << ' '
              << formatNumber(3.14159, 2, ',') << '\n';

    Button b;
    b.click(1);
    b.onClick = report;
    b.click(5);
    // Lambda bez hvatanja ima implicitnu konverziju u pokazivač na funkciju.
    // Sa hvatanjem ([&]) ne bi mogla (errors/e09) -- tada std::function.
    b.onClick = [](int x) { std::cout << "lambda: " << x * 10 << '\n'; };
    b.click(5);
}
