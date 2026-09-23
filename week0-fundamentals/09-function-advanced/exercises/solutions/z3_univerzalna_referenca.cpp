// Rešenje zadatka z3_univerzalna_referenca.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

// Ako overload-uješ const std::string& i T&& (nije dobro): T&& je "gladan"
// -- za nekonstantni string i za literal daje tačan match i pobedi.
// Treba ovako: ograniči template tako da ne učestvuje za tipove koji idu u
// std::string (SFINAE: enable_if uklanja kandidata iz izbora).
// U C++20 se isto piše čitljivije: requires (!std::is_convertible_v<T, std::string>).
void log(const std::string& s) { std::cout << "string: \"" << s << "\"\n"; }

template <typename T,
          std::enable_if_t<!std::is_convertible_v<T, std::string>, int> = 0>
void log(T&& x) { std::cout << "generički: " << std::forward<T>(x) << '\n'; }

// Korak 3: const T& rešava samo pola. Za nekonstantni string oba
// overload-a traže isto (vezivanje za const&), pa je izjednačeno, a na
// izjednačenju non-template pobeđuje -> "string". Ali za literal je
// const T& (T = char[8]) i dalje tačan match, a string overload traži
// korisničku konverziju -> i dalje "generički" (provereno, g++ i clang).
// T&& je gori jer se prilagodi SVAKOM argumentu bez ikakve konverzije.

int main() {
    std::string ime = "motor";
    const std::string konst = "konst";
    log(ime);
    log(konst);
    log("literal");
    log(42);
}
