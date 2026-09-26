// KIND: usage
//
// Zadatak 1 -- constexpr funkcija, tabela pri kompajliranju, static_assert
// i if constexpr (sekcije 1, 4, 5, 6)
//   ./build.sh 1-language-basics/12-constexpr/exercises/ex1_crc_table.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_crc_table.cpp
//
// CRC-8 (polinom 0x07, početna vrednost 0) -- čest u senzorima i
// protokolima. Kontrolna vrednost za "123456789" je 0xF4.
// Korak 1: constexpr std::uint8_t crc8(const char* d, std::size_t n) --
//   bit po bit: c ^= bajt, pa 8 puta: ako je najviši bit 1,
//   c = (c << 1) ^ 0x07, inače c = c << 1 (static_cast na uint8_t).
//   static_assert(crc8("123456789", 9) == 0xF4); -- test se izvrši pri
//   KOMPAJLIRANJU; pogrešna funkcija = program se ne kompajlira.
// Korak 2: constexpr std::array<std::uint8_t, 256> makeTable() --
//   table[i] = crc8 jednog bajta i. inline constexpr auto table =
//   makeTable(); pa crc8Fast(d, n) koji po bajtu radi
//   c = table[c ^ bajt]. static_assert da daje isto 0xF4.
// Korak 3: template <typename T> std::string toText(const T& v) sa
//   if constexpr: celi brojevi -> std::to_string, decimalni -> "x.yy"
//   (ostringstream, 2 decimale), sve ostalo -> std::string(v). Grane koje
//   nisu izabrane se ni ne kompajliraju za taj T.

#include <array>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

// TODO korak 1, 2, 3

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // std::cout << std::hex << "crc8: 0x" << int(crc8("123456789", 9))
    //           << ", fast: 0x" << int(crc8Fast("123456789", 9))
    //           << ", table[1]: 0x" << int(table[1]) << std::dec << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << toText(42) << ' ' << toText(3.14159) << ' ' << toText("text") << '\n';
}

/* EXPECTED OUTPUT
crc8: 0xf4, fast: 0xf4, table[1]: 0x7
42 3.14 text
*/
