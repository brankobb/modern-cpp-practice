// EXPECT-RUN: terminate called after throwing an instance of 'std::invalid_argument'
// NIJE UB, ali program se prekine: izuzetak koji izleti iz noexcept
// funkcije poziva std::terminate ([except.spec]). noexcept je OBEĆANJE
// koje kompajler ne proverava pri kompajliranju. Upozori samo na
// očigledan throw u telu (runtime/r02); ovde baca std::stoi, pa nema ni
// upozorenja (test). Pri izvršavanju obećanje se "proveri" tako što se
// program prekine.
// Ispravno: noexcept samo na funkcijama koje zaista ne bacaju (move
// operacije, swap, destruktori), ili uhvati sve unutar funkcije.
#include <iostream>
#include <stdexcept>
#include <string>
int parsiraj(const std::string& s) noexcept {
    return std::stoi(s);          // stoi baca za "abc"
}
int main() {
    try {
        std::cout << parsiraj("abc") << '\n';
    } catch (...) {
        std::cout << "uhvaćeno\n";   // nikad se ne izvrši
    }
}
