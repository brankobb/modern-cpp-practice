// Rešenje zadatka ex3_explicit.

#include <cstddef>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <vector>

// Ako konstruktor Paket(std::size_t) nije explicit (nije dobro): svaki
// broj se tiho pretvara u Paket, pa posalji(42) pošalje 42 prazna bajta.
// Treba ovako: explicit -- veličina se zadaje samo vidljivo, Paket(42).
// Konstruktor iz liste bajtova ostaje implicitan, jer je tu konverzija
// ono što se i misli: posalji({0x42}) je "paket sa bajtom 0x42".
class Paket {
public:
    explicit Paket(std::size_t velicina) : bajtovi_(velicina) {}
    Paket(std::initializer_list<unsigned char> b) : bajtovi_(b) {}

    std::size_t velicina() const { return bajtovi_.size(); }
    const std::vector<unsigned char>& bajtovi() const { return bajtovi_; }

private:
    std::vector<unsigned char> bajtovi_;
};

void posalji(const Paket& p) {
    std::cout << "šaljem paket od " << p.velicina() << " bajta:" << std::hex << std::setfill('0');
    for (unsigned char b : p.bajtovi()) std::cout << ' ' << std::setw(2) << int(b);
    std::cout << std::dec << '\n';
}

int main() {
    posalji(Paket(3));
    posalji({0x42});
    posalji({0x01, 0x02});
}
