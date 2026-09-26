// Rešenje zadatka ex2_destruktor_ukida_move.

#include <iostream>
#include <string>
#include <utility>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

struct Tekst {
    std::string s;
    explicit Tekst(std::string x) : s(std::move(x)) {}
    Tekst(const Tekst& o) : s(o.s) { ++brojac.kopija; }
    Tekst(Tekst&& o) noexcept : s(std::move(o.s)) { ++brojac.pomeranja; }
    Tekst& operator=(const Tekst&) = default;
    Tekst& operator=(Tekst&&) noexcept = default;
};

int unistenih = 0;

// Ako dodaš samo destruktor (nije dobro): move operacije se ne generišu,
// i svaki std::move tiho postane kopija.
// Treba ovako: kad deklarišeš bilo koju od pet, deklariši svih pet (C.21).
// = default zadrži ponašanje "član po član", a noexcept se izvede iz
// članova.
struct Poruka {
    Tekst t;
    explicit Poruka(Tekst x) : t(std::move(x)) {}
    ~Poruka() { ++unistenih; }
    Poruka(const Poruka&) = default;
    Poruka& operator=(const Poruka&) = default;
    Poruka(Poruka&&) = default;
    Poruka& operator=(Poruka&&) = default;
};

int main() {
    {
        Poruka p{Tekst("sadržaj")};
        brojac = Brojac{};                 // brojimo samo ono što uradi std::move
        Poruka q = std::move(p);
        (void)q;
        std::cout << "kopija: " << brojac.kopija << ", pomeranja: " << brojac.pomeranja << '\n';
    }
    std::cout << "uništenih: " << unistenih << '\n';
}
