// KIND: why
// DEMO-OUT: NAIVE timeout: 5 ms
//
// Zadatak 3 -- zašto jedinice u tipu i korisnički literali (sekcija 6)
// Rešenje: exercises/solutions/ex3_units_literal.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 1-language-basics/06-strings/exercises/ex3_units_literal.cpp -DNAIVE
//   setTimeout(int) očekuje milisekunde, a pozivalac je mislio na
//   sekunde: timeout je 1000 puta kraći. Iz "setTimeout(5)" se ne vidi
//   jedinica, pa kompajler nema šta da proveri.
// Korak 2: u #else grani napiši tip
//       struct Milliseconds { long long value; };
//   (sa explicit konstruktorom, da int ne prolazi sam), i korisničke
//   literale operator""_ms i operator""_s (parametar unsigned long long,
//   constexpr) koji oba vraćaju Milliseconds. setTimeout prima
//   Milliseconds. Poziv setTimeout(5) tada NE SME da se kompajlira.
// Korak 3: isto sa std::chrono (standard već ima ovo):
//   void setTimeoutChrono(std::chrono::milliseconds t) i poziv sa
//   5s (using namespace std::chrono_literals). Sekunde se same pretvore u
//   milisekunde, jer je to konverzija BEZ gubitka.

#include <chrono>
#include <iostream>

#ifdef NAIVE
void setTimeout(int ms) { std::cout << "timeout: " << ms << " ms\n"; }

int main() {
    setTimeout(5);         // hteo sam 5 sekundi
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // setTimeout(5_s);
    // setTimeout(250_ms);
    // using namespace std::chrono_literals;
    // setTimeoutChrono(5s);
    // setTimeoutChrono(250ms);
}
#endif

/* EXPECTED OUTPUT
timeout: 5000 ms
timeout: 250 ms
chrono timeout: 5000 ms
chrono timeout: 250 ms
*/
