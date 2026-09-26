// Rešenje zadatka ex3_explicit.

#include <cstddef>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <vector>

// Ako konstruktor Packet(std::size_t) nije explicit (nije dobro): svaki
// broj se tiho pretvara u Packet, pa send(42) pošalje 42 prazna bajta.
// Treba ovako: explicit -- veličina se zadaje samo vidljivo, Packet(42).
// Konstruktor iz liste bajtova ostaje implicitan, jer je tu konverzija
// ono što se i misli: send({0x42}) je "paket sa bajtom 0x42".
class Packet {
public:
    explicit Packet(std::size_t size) : bytes_(size) {}
    Packet(std::initializer_list<unsigned char> b) : bytes_(b) {}

    std::size_t size() const { return bytes_.size(); }
    const std::vector<unsigned char>& bytes() const { return bytes_; }

private:
    std::vector<unsigned char> bytes_;
};

void send(const Packet& p) {
    std::cout << "sending packet of size " << p.size() << ':' << std::hex << std::setfill('0');
    for (unsigned char b : p.bytes()) std::cout << ' ' << std::setw(2) << int(b);
    std::cout << std::dec << '\n';
}

int main() {
    send(Packet(3));
    send({0x42});
    send({0x01, 0x02});
}
