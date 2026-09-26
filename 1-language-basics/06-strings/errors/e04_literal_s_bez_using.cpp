// STD: c++17
// EXPECT-GCC: unable to find string literal operator 'operator""s'
// EXPECT-CLANG: no matching literal operator for call to 'operator""s'
// POGREŠNO: "tekst"s bez using namespace std::string_literals.
// Zašto: standardni literali su u (inline) namespace-ima std::literals::*
//   (lekcija 08, sekcija 6), pa ih nekvalifikovano traženje ne vidi dok se
//   ne uvedu. Tako "s" ne može slučajno da se sudari sa tuđim literalom.
// Ispravno: using namespace std::string_literals; u funkciji ili .cpp
//   fajlu (ne u header-u, SF.7), pa "tekst"s.
#include <string>

int main() {
    auto s = "tekst"s;
    return static_cast<int>(s.size());
}
