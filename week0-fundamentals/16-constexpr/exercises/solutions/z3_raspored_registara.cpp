// Rešenje zadatka z3_raspored_registara.

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

struct Paket {
    std::uint8_t tip;
    std::uint32_t vrednost;
    std::uint8_t status;
};

// Ako pretpostaviš da struktura ima raspored kao na žici (nije dobro):
// kompajler ubaci popunu (ovde 3 bajta posle tip i 3 posle status), pa
// memcpy sa žice napuni pogrešna polja -- bez ikakve greške.
// Treba ovako: pretpostavke o rasporedu zapiši kao static_assert (ovde
// bi pale, pa ih ne pišemo za Paket), a podatke sa žice čitaj bajt po
// bajt i sastavi polja sam. Tada raspored strukture nije bitan.
static_assert(alignof(std::uint32_t) == 4, "vežba pretpostavlja 4-bajtno poravnanje");

Paket procitaj(const std::array<std::uint8_t, 6>& b) {
    Paket p{};
    p.tip = b[0];
    p.vrednost = static_cast<std::uint32_t>(b[1]) | static_cast<std::uint32_t>(b[2]) << 8 |
                 static_cast<std::uint32_t>(b[3]) << 16 | static_cast<std::uint32_t>(b[4]) << 24;
    p.status = b[5];
    return p;
}

int main() {
    std::cout << "sizeof: " << sizeof(Paket) << ", offsetof: tip " << offsetof(Paket, tip)
              << ", vrednost " << offsetof(Paket, vrednost) << ", status "
              << offsetof(Paket, status) << '\n';

    std::array<std::uint8_t, 6> sazice{0x01, 0x78, 0x56, 0x34, 0x12, 0x80};
    Paket p = procitaj(sazice);
    std::cout << std::hex << "tip 0x" << int(p.tip) << ", vrednost 0x" << p.vrednost
              << ", status 0x" << int(p.status) << '\n';
}
