// EXPECT-UB: heap-use-after-free
// UB: pogled pokazuje na bafer stringa. += je produžio string preko
// kapaciteta, string je alocirao nov bafer i oslobodio stari -- sv i
// dalje gleda u stari. Isto kao iterator ili pokazivač u vector posle
// push_back (lekcija 04, sekcija 12).
// Ispravno: pravi pogled POSLE poslednje izmene, ili drži poziciju i
// dužinu (indekse) umesto pogleda.
#include <iostream>
#include <string>
#include <string_view>
int main() {
    std::string s = "temperature sensor";
    std::string_view sv = s;
    s += " in hall no. 3, second floor";
    std::cout << sv.size() << ' ' << sv[0] << '\n';
}
