// VRSTA: zašto
// DEMO-OUT: NAIVNO vrednost: motor
//
// Zadatak 2 -- zašto se tip "očisti" pre pitanja traitu (sekcija 7)
// Rešenje: exercises/solutions/z2_trait_i_referenca.cpp
//
// jeString<T> je sopstveni trait: true samo za std::string. loguj(T&&)
// prima sve (forwarding referenca) i stringove ispisuje pod navodnicima.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 4-sabloni/28-klasni-sabloni-i-traits/exercises/z2_trait_i_referenca.cpp -DNAIVNO
//   Samo privremeni string je prepoznat kao tekst. Za imenovani string T je
//   std::string& (lvalue u T&&, lekcija 27), za const string T je const std::string&
//   -- a jeString<std::string&> i jeString<const std::string&> su DRUGI
//   tipovi od jeString<std::string>, pa pada u primarni šablon (false).
//   Specijalizacija se poklapa samo sa TAČNO tim tipom.
// Korak 2: u #else grani pitaj trait za tip bez reference i const-a:
//   std::remove_cv_t<std::remove_reference_t<T>> (C++20:
//   std::remove_cvref_t<T>), ili std::decay_t<T> (i niz -> pokazivač).
//   Napiši pomoćnik template <typename T> inline constexpr bool
//   jeString_v = jeString<...>::value; i koristi njega.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

template <typename T>
struct jeString : std::false_type {};
template <>
struct jeString<std::string> : std::true_type {};

#ifdef NAIVNO
template <typename T>
inline constexpr bool jeString_v = jeString<T>::value;
#else
// TODO korak 2 (dok ne napišeš, ovo je naivna verzija)
template <typename T>
inline constexpr bool jeString_v = jeString<T>::value;
#endif

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

/* OČEKIVANI IZLAZ
tekst: "motor"
tekst: "konstanta"
tekst: "privremeni"
vrednost: 42
*/
