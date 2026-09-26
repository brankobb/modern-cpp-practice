// Rešenje zadatka ex3_jedinice_literal.

#include <chrono>
#include <iostream>

// Ako funkcija prima "int ms" (nije dobro): jedinica postoji samo u imenu
// parametra, a na mestu poziva se ne vidi -- 5 može biti bilo šta.
// Treba ovako: jedinica je deo tipa, a literal je piše na mestu poziva.
struct Milisekunde {
    constexpr explicit Milisekunde(long long v) : vrednost(v) {}
    long long vrednost;
};

// Sufiks mora da počne sa _ (bez _ su rezervisani za standard).
constexpr Milisekunde operator""_ms(unsigned long long v) {
    return Milisekunde(static_cast<long long>(v));
}
constexpr Milisekunde operator""_s(unsigned long long v) {
    return Milisekunde(static_cast<long long>(v) * 1000);
}

void postaviTimeout(Milisekunde t) { std::cout << "timeout: " << t.vrednost << " ms\n"; }

// Možeš i ovako: std::chrono radi isto, uz proveru konverzija -- s -> ms
// je implicitna (bez gubitka), a ms -> s bi tražila duration_cast.
void postaviTimeoutChrono(std::chrono::milliseconds t) {
    std::cout << "chrono timeout: " << t.count() << " ms\n";
}

int main() {
#ifdef LOS_POZIV
    postaviTimeout(5);   // greška: explicit konstruktor, int ne prolazi
#endif
    postaviTimeout(5_s);
    postaviTimeout(250_ms);
    using namespace std::chrono_literals;
    postaviTimeoutChrono(5s);
    postaviTimeoutChrono(250ms);
}
