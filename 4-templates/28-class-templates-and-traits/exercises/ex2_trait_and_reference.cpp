// KIND: why
// DEMO-OUT: NAIVE value: motor
//
// Zadatak 2 -- zašto se tip "očisti" pre pitanja traitu (sekcija 7)
// Rešenje: exercises/solutions/ex2_trait_and_reference.cpp
//
// isString<T> je sopstveni trait: true samo za std::string. logValue(T&&)
// prima sve (forwarding referenca) i stringove ispisuje pod navodnicima.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-templates/28-class-templates-and-traits/exercises/ex2_trait_and_reference.cpp -DNAIVE
//   Samo privremeni string je prepoznat kao tekst. Za imenovani string T je
//   std::string& (lvalue u T&&, lekcija 27), za const string T je const std::string&
//   -- a isString<std::string&> i isString<const std::string&> su DRUGI
//   tipovi od isString<std::string>, pa pada u primarni šablon (false).
//   Specijalizacija se poklapa samo sa TAČNO tim tipom.
// Korak 2: u #else grani pitaj trait za tip bez reference i const-a:
//   std::remove_cv_t<std::remove_reference_t<T>> (C++20:
//   std::remove_cvref_t<T>), ili std::decay_t<T> (i niz -> pokazivač).
//   Napiši pomoćnik template <typename T> inline constexpr bool
//   isString_v = isString<...>::value; i koristi njega.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
struct isString : std::false_type {};
template <>
struct isString<std::string> : std::true_type {};

#ifdef NAIVE
template <typename T>
inline constexpr bool isString_v = isString<T>::value;
#else
// TODO korak 2 (dok ne napišeš, ovo je naivna verzija)
template <typename T>
inline constexpr bool isString_v = isString<T>::value;
#endif

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

/* EXPECTED OUTPUT
text: "motor"
text: "constant"
text: "temporary"
value: 42
*/
