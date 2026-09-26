// KIND: why
// DEMO-OUT: NAIVE copies during growth: [1-9]
//
// Zadatak 3 -- zašto move konstruktor treba noexcept (sekcija 4, EMC Item 14)
// Rešenje: exercises/solutions/ex3_noexcept_vector.cpp
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/23-special-member-generation/exercises/ex3_noexcept_vector.cpp -DNAIVE
//   Sample ima ručno napisan move konstruktor, ali BEZ noexcept. Kad
//   vector raste, stare elemente KOPIRA u novi blok, iako move postoji.
//   Razlog: push_back daje jaku garanciju (lekcija 21). Ako bi move usred
//   premeštanja bacio, stari blok bi već bio delimično "opljačkan" i ne bi
//   mogao da se vrati. Kopiranje ostavlja original netaknut, pa vector
//   koristi move samo kad je noexcept (std::move_if_noexcept).
//   (Tačan broj kopija zavisi od toga koliko puta vector raste -- to
//   zavisi od biblioteke.)
// Korak 2: u #else grani napiši Sample sa noexcept move konstruktorom (ili
//   ga uopšte ne piši -- generisani je noexcept kad su move-ovi članova
//   noexcept). Proveri static_assert-om:
//   static_assert(std::is_nothrow_move_constructible_v<Sample>);

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct Counter {
    int copies = 0;
    int moves = 0;
} counter;

#ifdef NAIVE
struct Sample {
    std::string name;
    explicit Sample(std::string n) : name(std::move(n)) {}
    Sample(const Sample& o) : name(o.name) { ++counter.copies; }
    Sample(Sample&& o) : name(std::move(o.name)) { ++counter.moves; }   // bez noexcept
};
#else
// TODO korak 2 (dok ne napišeš, ovo je naivna verzija bez brojanja)
struct Sample {
    std::string name;
    explicit Sample(std::string n) : name(std::move(n)) {}
};
#endif

int main() {
    std::vector<Sample> v;
    for (int i = 0; i < 20; ++i) v.emplace_back("sample-" + std::to_string(i));   // pravi na mestu
    std::cout << "copies during growth: " << counter.copies << '\n';
    std::cout << "moves during growth: " << (counter.moves > 0 ? "yes" : "no") << '\n';
}

/* EXPECTED OUTPUT
copies during growth: 0
moves during growth: yes
*/
