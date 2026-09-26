// Rešenje zadatka ex2_trait_i_referenca.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
struct jeString : std::false_type {};
template <>
struct jeString<std::string> : std::true_type {};

// Ako pitaš trait direktno za T iz forwarding reference (nije dobro): T je
// std::string& ili const std::string&, a specijalizacija postoji samo za
// std::string -- pa je odgovor "nije string".
// Treba ovako: ukloni referencu i const/volatile pre pitanja.
template <typename T>
inline constexpr bool jeString_v = jeString<std::remove_cv_t<std::remove_reference_t<T>>>::value;

template <typename T>
void loguj(T&& x) {
    if constexpr (jeString_v<T>)
        std::cout << "tekst: \"" << x << "\"\n";
    else
        std::cout << "vrednost: " << x << '\n';
}

int main() {
    std::string ime = "motor";
    const std::string konst = "konstanta";
    loguj(ime);
    loguj(konst);
    loguj(std::string("privremeni"));
    loguj(42);
}
