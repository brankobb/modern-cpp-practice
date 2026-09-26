// Rešenje zadatka ex1_crc_table.

#include <array>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>

// Korak 1: constexpr funkcija SME da se izvrši pri kompajliranju (kad su
// argumenti konstante), a radi i pri izvršavanju.
constexpr std::uint8_t crc8(const char* d, std::size_t n) {
    std::uint8_t c = 0;
    for (std::size_t i = 0; i < n; ++i) {
        c ^= static_cast<std::uint8_t>(d[i]);
        for (int b = 0; b < 8; ++b)
            c = (c & 0x80) ? static_cast<std::uint8_t>((c << 1) ^ 0x07)
                           : static_cast<std::uint8_t>(c << 1);
    }
    return c;
}
static_assert(crc8("123456789", 9) == 0xF4, "CRC-8 check value");

// Korak 2: tabela se izračuna pri kompajliranju i završi u .rodata (na
// mikrokontroleru u flash-u) -- nula posla pri startu, nula RAM-a.
constexpr std::array<std::uint8_t, 256> makeTable() {
    std::array<std::uint8_t, 256> t{};
    for (std::size_t i = 0; i < 256; ++i) {
        char byte = static_cast<char>(i);
        t[i] = crc8(&byte, 1);
    }
    return t;
}
inline constexpr auto table = makeTable();

constexpr std::uint8_t crc8Fast(const char* d, std::size_t n) {
    std::uint8_t c = 0;
    for (std::size_t i = 0; i < n; ++i) c = table[c ^ static_cast<std::uint8_t>(d[i])];
    return c;
}
static_assert(crc8Fast("123456789", 9) == 0xF4, "the table gives the same result");

// Korak 3: if constexpr bira granu pri kompajliranju. Sa običnim if bi
// se za T = const char[5] kompajlirao i std::to_string(v) -- greška.
template <typename T>
std::string toText(const T& v) {
    if constexpr (std::is_integral_v<T>) {
        return std::to_string(v);
    } else if constexpr (std::is_floating_point_v<T>) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(2) << v;
        return os.str();
    } else {
        return std::string(v);
    }
}

int main() {
    std::cout << std::hex << "crc8: 0x" << int(crc8("123456789", 9))
              << ", fast: 0x" << int(crc8Fast("123456789", 9))
              << ", table[1]: 0x" << int(table[1]) << std::dec << '\n';

    std::cout << toText(42) << ' ' << toText(3.14159) << ' ' << toText("text") << '\n';
}
