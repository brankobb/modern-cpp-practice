// Rešenje zadatka z2_override.

#include <iostream>

struct Senzor {
    virtual ~Senzor() = default;
    virtual double ocitaj() const {
        std::cout << "Senzor::ocitaj -- podrazumevano\n";
        return 0.0;
    }
};

// Ako izostaviš const (ili promeniš tip parametra) bez override (nije
// dobro): nastane nova funkcija, a virtualni poziv tiho ide u baznu klasu.
// Treba ovako: override na SVAKOJ funkciji koja nadjačava (C.128). Potpis
// koji se ne poklapa postaje greška pri kompajliranju. virtual se u
// izvedenoj klasi ne piše -- override ga podrazumeva.
struct TermoSenzor : Senzor {
    double ocitaj() const override {
        std::cout << "TermoSenzor::ocitaj\n";
        return 21.5;
    }
};

void izvestaj(const Senzor& s) {
    double v = s.ocitaj();   // prvo očitaj (ocitaj() i sam ispisuje), pa ispiši
    std::cout << "vrednost: " << v << '\n';
}

int main() {
    TermoSenzor t;
    izvestaj(t);
}
