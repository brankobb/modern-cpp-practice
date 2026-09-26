// KIND: why
// DEMO-OUT: NAIVNO "12abc" -> 12
//
// Zadatak 2 -- zašto atoi (i stoi bez provere) nije parsiranje (sekcija 4)
// Rešenje: exercises/solutions/ex2_from_chars.cpp
//
// Broj stiže kao tekst (serijski port, fajl). Pogrešan unos mora da se
// PRIMETI, a ne da se pretvori u neki broj.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/06-strings/exercises/ex2_from_chars.cpp -DNAIVNO
//   atoi("abc") je 0 -- isto kao atoi("0"), pa greška ne može da se
//   razlikuje od nule. atoi("12abc") je 12 (ostatak se tiho ignoriše), a
//   atoi van opsega int-a je UB. std::stoi baca izuzetak za "abc", ali
//   "12abc" i dalje prihvati kao 12 ako ne proveriš pos.
// Korak 2: u #else grani napiši bool parsiraj(const std::string& s, int& out)
//   sa std::from_chars (<charconv>, C++17): uspeh je SAMO kad je
//   ec == std::errc{} I ptr == kraj teksta (pročitan je CEO tekst). Za
//   vrednost van opsega from_chars vrati std::errc::result_out_of_range.
// Korak 3: otkomentariši test.

#include <charconv>
#include <cstdlib>
#include <iostream>
#include <string>
#include <system_error>

#ifdef NAIVNO
int main() {
    for (const char* s : {"42", "0", "abc", "12abc"})
        std::cout << '"' << s << "\" -> " << std::atoi(s) << '\n';
}
#else
// TODO korak 2

int main() {
    // Korak 3 -- otkomentariši:
    // for (const char* s : {"42", "0", "-7", "abc", "12abc", "", "99999999999"}) {
    //     int x = 0;
    //     if (parsiraj(s, x))
    //         std::cout << '"' << s << "\" -> " << x << '\n';
    //     else
    //         std::cout << '"' << s << "\" -> greška\n";
    // }
}
#endif

/* EXPECTED OUTPUT
"42" -> 42
"0" -> 0
"-7" -> -7
"abc" -> greška
"12abc" -> greška
"" -> greška
"99999999999" -> greška
*/
