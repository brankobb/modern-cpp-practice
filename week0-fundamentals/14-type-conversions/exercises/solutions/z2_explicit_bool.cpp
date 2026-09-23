// Rešenje zadatka z2_explicit_bool.

#include <iostream>

// Ako je operator bool() bez explicit (nije dobro): Konekcija se tiho
// pretvara u bool, pa u int -- "k1 == k2" poredi true sa true, a "k1 + 10"
// je 11. Nijedno nema smisla, a oba se kompajliraju.
// Treba ovako: explicit operator bool radi samo u kontekstu bool-a (if,
// while, !, &&, ||, ?:), kao kod unique_ptr, optional i stream-ova. Za
// poređenje napiši pravi operator==.
class Konekcija {
public:
    explicit Konekcija(int port) : port_(port) {}
    explicit operator bool() const { return port_ != 0; }
    friend bool operator==(const Konekcija& a, const Konekcija& b) { return a.port_ == b.port_; }

private:
    int port_;
};

int main() {
    Konekcija k1(80), k2(443), k3(80), zatvorena(0);
    if (k1) std::cout << "k1 otvorena\n";
    if (!zatvorena) std::cout << "zatvorena nije otvorena\n";
    std::cout << "k1 == k2: " << (k1 == k2) << ", k1 == k3: " << (k1 == k3) << '\n';
    bool b = static_cast<bool>(k2);
    std::cout << "k2 kao bool: " << b << '\n';
}
