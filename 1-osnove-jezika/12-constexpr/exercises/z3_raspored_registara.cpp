// VRSTA: zašto
// DEMO-ERR: NAIVNO static assertion failed|static_assert failed
//
// Zadatak 3 -- zašto static_assert za pretpostavke o rasporedu u memoriji
// (sekcija 6)
// Rešenje: exercises/solutions/z3_raspored_registara.cpp
//
// Struktura opisuje paket koji uređaj šalje: 1 bajt tip, 4 bajta vrednost,
// 1 bajt status -- ukupno 6 bajtova, tim redom.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-osnove-jezika/12-constexpr/exercises/z3_raspored_registara.cpp -DNAIVNO
//   static_assert(sizeof(Paket) == 6) padne: kompajler je ubacio
//   PADDING (popunu) da bi uint32_t bio poravnat na 4 bajta. Bez
//   static_assert-a bi kod koji čita 6 bajtova sa žice u ovu strukturu
//   (memcpy) tiho pročitao pogrešna polja.
// Korak 2: koliki je sizeof i gde je koje polje? Ispiši offsetof
//   (<cstddef>) za sva tri polja (otkomentariši test).
// Korak 3: struktura ne može da opiše 6 bajtova bez popune (uint32_t mora
//   biti poravnat). Ispravno rešenje za protokol: bajtovi se čitaju u niz
//   std::array<std::uint8_t, 6>, a polja se sastave ručno (npr. vrednost =
//   b[1] | b[2] << 8 | b[3] << 16 | b[4] << 24, little endian). Napiši
//   Paket procitaj(const std::array<std::uint8_t, 6>& b).

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

struct Paket {
    std::uint8_t tip;
    std::uint32_t vrednost;
    std::uint8_t status;
};

#ifdef NAIVNO
static_assert(sizeof(Paket) == 6, "Paket mora da ima tačno 6 bajtova, kao na žici");
#endif

// TODO korak 3

int main() {
    // Korak 2 -- otkomentariši:
    // std::cout << "sizeof: " << sizeof(Paket) << ", offsetof: tip " << offsetof(Paket, tip)
    //           << ", vrednost " << offsetof(Paket, vrednost) << ", status "
    //           << offsetof(Paket, status) << '\n';

    // Korak 3 -- otkomentariši:
    // std::array<std::uint8_t, 6> sazice{0x01, 0x78, 0x56, 0x34, 0x12, 0x80};
    // Paket p = procitaj(sazice);
    // std::cout << std::hex << "tip 0x" << int(p.tip) << ", vrednost 0x" << p.vrednost
    //           << ", status 0x" << int(p.status) << '\n';
}

/* OČEKIVANI IZLAZ
sizeof: 12, offsetof: tip 0, vrednost 4, status 8
tip 0x1, vrednost 0x12345678, status 0x80
*/
