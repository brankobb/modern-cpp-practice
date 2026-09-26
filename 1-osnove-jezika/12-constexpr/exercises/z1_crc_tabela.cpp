// VRSTA: upotreba
//
// Zadatak 1 -- constexpr funkcija, tabela pri kompajliranju, static_assert
// i if constexpr (sekcije 1, 4, 5, 6)
//   ./build.sh 1-osnove-jezika/12-constexpr/exercises/z1_crc_tabela.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_crc_tabela.cpp
//
// CRC-8 (polinom 0x07, početna vrednost 0) -- čest u senzorima i
// protokolima. Kontrolna vrednost za "123456789" je 0xF4.
// Korak 1: constexpr std::uint8_t crc8(const char* d, std::size_t n) --
//   bit po bit: c ^= bajt, pa 8 puta: ako je najviši bit 1,
//   c = (c << 1) ^ 0x07, inače c = c << 1 (static_cast na uint8_t).
//   static_assert(crc8("123456789", 9) == 0xF4); -- test se izvrši pri
//   KOMPAJLIRANJU; pogrešna funkcija = program se ne kompajlira.
// Korak 2: constexpr std::array<std::uint8_t, 256> napraviTabelu() --
//   tabela[i] = crc8 jednog bajta i. inline constexpr auto tabela =
//   napraviTabelu(); pa crc8Brzo(d, n) koji po bajtu radi
//   c = tabela[c ^ bajt]. static_assert da daje isto 0xF4.
// Korak 3: template <typename T> std::string uTekst(const T& v) sa
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
    //           << ", brzo: 0x" << int(crc8Brzo("123456789", 9))
    //           << ", tabela[1]: 0x" << int(tabela[1]) << std::dec << '\n';

    // Korak 3 -- otkomentariši:
    // std::cout << uTekst(42) << ' ' << uTekst(3.14159) << ' ' << uTekst("tekst") << '\n';
}

/* OČEKIVANI IZLAZ
crc8: 0xf4, brzo: 0xf4, tabela[1]: 0x7
42 3.14 tekst
*/
