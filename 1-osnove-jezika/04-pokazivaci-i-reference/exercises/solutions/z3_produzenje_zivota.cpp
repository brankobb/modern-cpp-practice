// Rešenje zadatka z3_produzenje_zivota.

#include <iostream>

const int& veci(const int& a, const int& b) { return a > b ? a : b; }

// Referenca na izlazu je u redu kad pokazuje na objekat koji je živeo PRE
// poziva (ovde: pozivaočeve promenljive), i koji će živeti posle.
int& veciRef(int& a, int& b) { return a > b ? a : b; }

int main() {
    int x = 3, y = 7;
    // Ako radiš "const int& m = veci(x + 1, y + 1);" (nije dobro): x + 1 i
    // y + 1 su privremeni koji nestaju na kraju izraza, a m ostane vezan za
    // jedan od njih. Treba ovako: sačuvaj po vrednosti (int je jeftin).
    int m = veci(x + 1, y + 1);
    std::cout << "veci: " << m << '\n';
    veciRef(x, y) += 10;
    std::cout << "x=" << x << " y=" << y << '\n';
}
