// Rešenje zadatka ex2_explicit_bool.

#include <iostream>

// Ako je operator bool() bez explicit (nije dobro): Connection se tiho
// pretvara u bool, pa u int -- "c1 == c2" poredi true sa true, a "c1 + 10"
// je 11. Nijedno nema smisla, a oba se kompajliraju.
// Treba ovako: explicit operator bool radi samo u kontekstu bool-a (if,
// while, !, &&, ||, ?:), kao kod unique_ptr, optional i stream-ova. Za
// poređenje napiši pravi operator==.
class Connection {
public:
    explicit Connection(int port) : port_(port) {}
    explicit operator bool() const { return port_ != 0; }
    friend bool operator==(const Connection& a, const Connection& b) { return a.port_ == b.port_; }

private:
    int port_;
};

int main() {
    Connection c1(80), c2(443), c3(80), closed(0);
    if (c1) std::cout << "c1 open\n";
    if (!closed) std::cout << "closed is not open\n";
    std::cout << "c1 == c2: " << (c1 == c2) << ", c1 == c3: " << (c1 == c3) << '\n';
    bool b = static_cast<bool>(c2);
    std::cout << "c2 as bool: " << b << '\n';
}
