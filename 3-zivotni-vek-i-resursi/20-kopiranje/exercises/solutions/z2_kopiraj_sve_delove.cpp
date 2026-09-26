// Rešenje zadatka z2_kopiraj_sve_delove.

#include <iostream>
#include <string>
#include <utility>

int kopija = 0;

struct Uredjaj {
    std::string ime;
    Uredjaj() : ime("bez imena") {}
    explicit Uredjaj(std::string i) : ime(std::move(i)) {}
};

// Ako ručna kopija izostavi baznu klasu (nije dobro): copy konstruktor
// napravi bazu podrazumevanim konstruktorom, a dodela je ne dira -- kopija
// tiho izgubi deo stanja.
// Treba ovako: u init listi copy konstruktora Uredjaj(o), a u dodeli
// Uredjaj::operator=(o). (Senzor& se implicitno konvertuje u Uredjaj&.)
struct Senzor : Uredjaj {
    double kal;
    Senzor(std::string i, double k) : Uredjaj(std::move(i)), kal(k) {}
    Senzor(const Senzor& o) : Uredjaj(o), kal(o.kal) { ++kopija; }
    Senzor& operator=(const Senzor& o) {
        Uredjaj::operator=(o);
        kal = o.kal;
        ++kopija;
        return *this;
    }
};

// Korak 3: bez brojača ne piši ništa -- generisane kopije kopiraju SVE
// (baze i sve članove), i ne mogu da zaborave novi član dodat kasnije.

int main() {
    Senzor a("temperatura", 1.5);
    Senzor b(a);
    std::cout << "kopija: " << b.ime << ", kal " << b.kal << '\n';
    Senzor c("pritisak", 0.5);
    c = a;
    std::cout << "dodela: " << c.ime << ", kal " << c.kal << '\n';
    std::cout << "kopiranja: " << kopija << '\n';
}
