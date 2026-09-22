// STD: c++17
// EXPECT-GCC: as 'this' argument discards qualifiers
// EXPECT-CLANG: no viable overloaded operator[]
// POGREŠNO: std::map::operator[] ne postoji za const mapu -- on UBACUJE
// element kad ključ ne postoji, a to je izmena.
// Ispravno: m.at("a") (baca izuzetak ako nema ključa) ili m.find("a").
#include <map>
#include <string>
int main() {
    const std::map<std::string, int> m{{"a", 1}};
    return m["a"];
}
