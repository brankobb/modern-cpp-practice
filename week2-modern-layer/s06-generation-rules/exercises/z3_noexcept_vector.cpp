// VRSTA: zašto
// DEMO-OUT: NAIVNO kopija pri rastu: [1-9]
//
// Zadatak 3 -- zašto move konstruktor treba noexcept (sekcija 4, EMC Item 14)
// Rešenje: exercises/solutions/z3_noexcept_vector.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week2-modern-layer/s06-generation-rules/exercises/z3_noexcept_vector.cpp -DNAIVNO
//   Uzorak ima ručno napisan move konstruktor, ali BEZ noexcept. Kad
//   vector raste, stare elemente KOPIRA u novi blok, iako move postoji.
//   Razlog: push_back daje jaku garanciju (s03). Ako bi move usred
//   premeštanja bacio, stari blok bi već bio delimično "opljačkan" i ne bi
//   mogao da se vrati. Kopiranje ostavlja original netaknut, pa vector
//   koristi move samo kad je noexcept (std::move_if_noexcept).
//   (Tačan broj kopija zavisi od toga koliko puta vector raste -- to
//   zavisi od biblioteke.)
// Korak 2: u #else grani napiši Uzorak sa noexcept move konstruktorom (ili
//   ga uopšte ne piši -- generisani je noexcept kad su move-ovi članova
//   noexcept). Proveri static_assert-om:
//   static_assert(std::is_nothrow_move_constructible_v<Uzorak>);

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Brojac {
    int kopija = 0;
    int pomeranja = 0;
} brojac;

#ifdef NAIVNO
struct Uzorak {
    std::string ime;
    explicit Uzorak(std::string i) : ime(std::move(i)) {}
    Uzorak(const Uzorak& o) : ime(o.ime) { ++brojac.kopija; }
    Uzorak(Uzorak&& o) : ime(std::move(o.ime)) { ++brojac.pomeranja; }   // bez noexcept
};
#else
// TODO korak 2 (dok ne napišeš, ovo je naivna verzija bez brojanja)
struct Uzorak {
    std::string ime;
    explicit Uzorak(std::string i) : ime(std::move(i)) {}
};
#endif

int main() {
    std::vector<Uzorak> v;
    for (int i = 0; i < 20; ++i) v.emplace_back("uzorak-" + std::to_string(i));   // pravi na mestu
    std::cout << "kopija pri rastu: " << brojac.kopija << '\n';
    std::cout << "pomeranja pri rastu: " << (brojac.pomeranja > 0 ? "da" : "ne") << '\n';
}

/* OČEKIVANI IZLAZ
kopija pri rastu: 0
pomeranja pri rastu: da
*/
