// Rešenje zadatka ex3_units_literal.

#include <chrono>
#include <iostream>

// Ako funkcija prima "int ms" (nije dobro): jedinica postoji samo u imenu
// parametra, a na mestu poziva se ne vidi -- 5 može biti bilo šta.
// Treba ovako: jedinica je deo tipa, a literal je piše na mestu poziva.
struct Milliseconds {
    constexpr explicit Milliseconds(long long v) : value(v) {}
    long long value;
};

// Sufiks mora da počne sa _ (bez _ su rezervisani za standard).
constexpr Milliseconds operator""_ms(unsigned long long v) {
    return Milliseconds(static_cast<long long>(v));
}
constexpr Milliseconds operator""_s(unsigned long long v) {
    return Milliseconds(static_cast<long long>(v) * 1000);
}

void setTimeout(Milliseconds t) { std::cout << "timeout: " << t.value << " ms\n"; }

// Možeš i ovako: std::chrono radi isto, uz proveru konverzija -- s -> ms
// je implicitna (bez gubitka), a ms -> s bi tražila duration_cast.
void setTimeoutChrono(std::chrono::milliseconds t) {
    std::cout << "chrono timeout: " << t.count() << " ms\n";
}

int main() {
#ifdef BAD_CALL
    setTimeout(5);   // greška: explicit konstruktor, int ne prolazi
#endif
    setTimeout(5_s);
    setTimeout(250_ms);
    using namespace std::chrono_literals;
    setTimeoutChrono(5s);
    setTimeoutChrono(250ms);
}
