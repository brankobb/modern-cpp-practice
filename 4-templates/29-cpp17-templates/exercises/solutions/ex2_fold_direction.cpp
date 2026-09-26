// Rešenje zadatka ex2_fold_direction.

#include <iostream>

// Ako je (a - ...) (nije dobro): desni fold, a1 - (a2 - (a3 - ...)) --
// oduzimanje nije asocijativno, pa je rezultat pogrešan.
// Treba ovako: levi fold, ((a1 - a2) - a3) - ...
template <typename... T>
int remaining(T... a) { return (... - a); }

// Korak 3: remaining(100) je 100 -- fold sa jednim elementom je sam taj
// element. remaining() se ne kompajlira: unarni fold praznog paketa
// postoji samo za &&, || i zarez (lekcija 28, errors/e05). Za budžet je bolje
// da on bude poseban parametar: int remaining(int budget, T... t)
// { return (budget - ... - t); } -- binarni levi fold, radi i bez troškova.

int main() {
    std::cout << "remaining from 100 after 30 and 20: " << remaining(100, 30, 20) << '\n';
    std::cout << "remaining from 100 after 5, 5 and 10: " << remaining(100, 5, 5, 10) << '\n';
}
