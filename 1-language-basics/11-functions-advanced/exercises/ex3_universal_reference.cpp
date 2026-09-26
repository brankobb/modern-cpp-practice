// KIND: why
// DEMO-OUT: NAIVE generic: engine
//
// Zadatak 3 -- zašto ne overload-ovati sa univerzalnom referencom
// (sekcija 4, EMC Item 26)
// Rešenje: exercises/solutions/ex3_universal_reference.cpp
//
// log() ima poseban overload za std::string (npr. dodaje navodnike), i
// generički za sve ostalo.
//
// Korak 1: PREDVIDI koji overload bira svaki od četiri poziva, pa pokreni:
//     ./build.sh 1-language-basics/11-functions-advanced/exercises/ex3_universal_reference.cpp -DNAIVE
//   Samo const std::string ide u string overload. Za nekonstantni string,
//   T&& se dedukuje u std::string& -- TAČAN match, bolji od
//   const std::string& (koji traži dodavanje const). Za literal, T&& se
//   veže direktno za const char(&)[8], a string overload traži
//   korisničku konverziju.
// Korak 2: u #else grani spreči da generički overload "otme" stringove:
//   ograniči ga (C++17) sa
//       template <typename T,
//                 std::enable_if_t<!std::is_convertible_v<T, std::string>, int> = 0>
//   Sve što može u std::string sada ide u string overload.
// Korak 3: (za razmišljanje) da li bi const T& umesto T&& rešio problem?
//   Za koje od četiri poziva? Predvidi, pa proveri u zasebnom fajlu.

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

#ifdef NAIVE
void log(const std::string& s) { std::cout << "string: \"" << s << "\"\n"; }
template <typename T>
void log(T&& x) { std::cout << "generic: " << std::forward<T>(x) << '\n'; }
#else
// TODO korak 2
#endif

int main() {
    std::string name = "engine";
    const std::string fixed = "fixed";
    (void)name;
    (void)fixed;
#ifdef NAIVE
    log(name);
    log(fixed);
    log("literal");
    log(42);
#else
    // Korak 2 -- otkomentariši:
    // log(name);
    // log(fixed);
    // log("literal");
    // log(42);
#endif
}

/* EXPECTED OUTPUT
string: "engine"
string: "fixed"
string: "literal"
generic: 42
*/
