// Rešenje zadatka ex2_smer_folda.

#include <iostream>

// Ako je (a - ...) (nije dobro): desni fold, a1 - (a2 - (a3 - ...)) --
// oduzimanje nije asocijativno, pa je rezultat pogrešan.
// Treba ovako: levi fold, ((a1 - a2) - a3) - ...
template <typename... T>
int preostalo(T... a) { return (... - a); }

// Korak 3: preostalo(100) je 100 -- fold sa jednim elementom je sam taj
// element. preostalo() se ne kompajlira: unarni fold praznog paketa
// postoji samo za &&, || i zarez (lekcija 28, errors/e05). Za budžet je bolje
// da on bude poseban parametar: int preostalo(int budzet, T... t)
// { return (budzet - ... - t); } -- binarni levi fold, radi i bez troškova.

int main() {
    std::cout << "preostalo od 100 posle 30 i 20: " << preostalo(100, 30, 20) << '\n';
    std::cout << "preostalo od 100 posle 5, 5 i 10: " << preostalo(100, 5, 5, 10) << '\n';
}
