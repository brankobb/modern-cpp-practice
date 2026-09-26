// Rešenje zadatka ex2_trait_and_reference.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
struct isString : std::false_type {};
template <>
struct isString<std::string> : std::true_type {};

// Ako pitaš trait direktno za T iz forwarding reference (nije dobro): T je
// std::string& ili const std::string&, a specijalizacija postoji samo za
// std::string -- pa je odgovor "nije string".
// Treba ovako: ukloni referencu i const/volatile pre pitanja.
template <typename T>
inline constexpr bool isString_v = isString<std::remove_cv_t<std::remove_reference_t<T>>>::value;

template <typename T>
void logValue(T&& x) {
    if constexpr (isString_v<T>)
        std::cout << "text: \"" << x << "\"\n";
    else
        std::cout << "value: " << x << '\n';
}

int main() {
    std::string name = "motor";
    const std::string constant = "constant";
    logValue(name);
    logValue(constant);
    logValue(std::string("temporary"));
    logValue(42);
}
