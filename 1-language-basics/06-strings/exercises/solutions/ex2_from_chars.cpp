// Rešenje zadatka ex2_from_chars.

#include <charconv>
#include <cstdlib>
#include <iostream>
#include <string>
#include <system_error>

// Ako koristiš atoi (nije dobro): greška izgleda kao 0, delimičan broj
// prođe, a prekoračenje je UB. stoi bez provere pos prihvata "12abc".
// Treba ovako: from_chars javi i grešku (ec) i gde je stao (ptr); uspeh
// je samo kad nema greške i kad je potrošen ceo tekst. Bez izuzetaka,
// bez alokacije, ne zavisi od locale-a.
bool parseInt(const std::string& s, int& out) {
    const char* end = s.data() + s.size();
    auto [ptr, ec] = std::from_chars(s.data(), end, out);
    return ec == std::errc{} && ptr == end;
}

int main() {
    for (const char* s : {"42", "0", "-7", "abc", "12abc", "", "99999999999"}) {
        int x = 0;
        if (parseInt(s, x))
            std::cout << '"' << s << "\" -> " << x << '\n';
        else
            std::cout << '"' << s << "\" -> error\n";
    }
}
