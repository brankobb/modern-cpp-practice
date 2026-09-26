// KIND: why
// DEMO-ERR: NAIVE static assertion failed|static_assert failed
//
// Zadatak 3 -- zašto static_assert za pretpostavke o rasporedu u memoriji
// (sekcija 6)
// Rešenje: exercises/solutions/ex3_register_layout.cpp
//
// Struktura opisuje paket koji uređaj šalje: 1 bajt tip, 4 bajta vrednost,
// 1 bajt status -- ukupno 6 bajtova, tim redom.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/12-constexpr/exercises/ex3_register_layout.cpp -DNAIVE
//   static_assert(sizeof(Packet) == 6) padne: kompajler je ubacio
//   PADDING (popunu) da bi uint32_t bio poravnat na 4 bajta. Bez
//   static_assert-a bi kod koji čita 6 bajtova sa žice u ovu strukturu
//   (memcpy) tiho pročitao pogrešna polja.
// Korak 2: koliki je sizeof i gde je koje polje? Ispiši offsetof
//   (<cstddef>) za sva tri polja (otkomentariši test).
// Korak 3: struktura ne može da opiše 6 bajtova bez popune (uint32_t mora
//   biti poravnat). Ispravno rešenje za protokol: bajtovi se čitaju u niz
//   std::array<std::uint8_t, 6>, a polja se sastave ručno (npr. value =
//   b[1] | b[2] << 8 | b[3] << 16 | b[4] << 24, little endian). Napiši
//   Packet parse(const std::array<std::uint8_t, 6>& b).

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

struct Packet {
    std::uint8_t type;
    std::uint32_t value;
    std::uint8_t status;
};

#ifdef NAIVE
static_assert(sizeof(Packet) == 6, "Packet must be exactly 6 bytes, as on the wire");
#endif

// TODO korak 3

int main() {
    // Korak 2 -- otkomentariši:
    // std::cout << "sizeof: " << sizeof(Packet) << ", offsetof: type " << offsetof(Packet, type)
    //           << ", value " << offsetof(Packet, value) << ", status "
    //           << offsetof(Packet, status) << '\n';

    // Korak 3 -- otkomentariši:
    // std::array<std::uint8_t, 6> fromWire{0x01, 0x78, 0x56, 0x34, 0x12, 0x80};
    // Packet p = parse(fromWire);
    // std::cout << std::hex << "type 0x" << int(p.type) << ", value 0x" << p.value
    //           << ", status 0x" << int(p.status) << '\n';
}

/* EXPECTED OUTPUT
sizeof: 12, offsetof: type 0, value 4, status 8
type 0x1, value 0x12345678, status 0x80
*/
