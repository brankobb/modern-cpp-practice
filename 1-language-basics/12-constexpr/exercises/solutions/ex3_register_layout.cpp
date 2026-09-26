// Rešenje zadatka ex3_register_layout.

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

struct Packet {
    std::uint8_t type;
    std::uint32_t value;
    std::uint8_t status;
};

// Ako pretpostaviš da struktura ima raspored kao na žici (nije dobro):
// kompajler ubaci popunu (ovde 3 bajta posle type i 3 posle status), pa
// memcpy sa žice napuni pogrešna polja -- bez ikakve greške.
// Treba ovako: pretpostavke o rasporedu zapiši kao static_assert (ovde
// bi pale, pa ih ne pišemo za Packet), a podatke sa žice čitaj bajt po
// bajt i sastavi polja sam. Tada raspored strukture nije bitan.
static_assert(alignof(std::uint32_t) == 4, "the exercise assumes 4-byte alignment");

Packet parse(const std::array<std::uint8_t, 6>& b) {
    Packet p{};
    p.type = b[0];
    p.value = static_cast<std::uint32_t>(b[1]) | static_cast<std::uint32_t>(b[2]) << 8 |
              static_cast<std::uint32_t>(b[3]) << 16 | static_cast<std::uint32_t>(b[4]) << 24;
    p.status = b[5];
    return p;
}

int main() {
    std::cout << "sizeof: " << sizeof(Packet) << ", offsetof: type " << offsetof(Packet, type)
              << ", value " << offsetof(Packet, value) << ", status "
              << offsetof(Packet, status) << '\n';

    std::array<std::uint8_t, 6> fromWire{0x01, 0x78, 0x56, 0x34, 0x12, 0x80};
    Packet p = parse(fromWire);
    std::cout << std::hex << "type 0x" << int(p.type) << ", value 0x" << p.value
              << ", status 0x" << int(p.status) << '\n';
}
