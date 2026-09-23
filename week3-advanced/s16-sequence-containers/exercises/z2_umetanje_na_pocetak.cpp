// VRSTA: zašto
// DEMO-OUT: NAIVNO pomeranja elemenata: [1-9][0-9][0-9]
//
// Zadatak 2 -- zašto vector nije za umetanje na početak (sekcije 3, 4)
// Rešenje: exercises/solutions/z2_umetanje_na_pocetak.cpp
//
// Poruka broji koliko puta je pomerena ili kopirana (svaki put kad
// kontejner premesti POSTOJEĆI element).
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh week3-advanced/s16-sequence-containers/exercises/z2_umetanje_na_pocetak.cpp -DNAIVNO
//   100 poruka umetnuto na početak vektora -- a elementi su pomereni
//   hiljadama puta (test, libstdc++: 5042). Svako umetanje na početak
//   pomeri sve postojeće za jedno mesto (0 + 1 + ... + 99 = 4950), uz
//   premeštanja pri rastu. Ukupno O(n^2): za 10 puta više poruka, oko 100
//   puta više posla. Program je tačan, samo spor -- i to se vidi tek kad n
//   poraste.
// Korak 2: u #else grani isto sa std::deque i emplace_front(i): element se
//   pravi na mestu, a postojeći se ne diraju. Očekuje se 0 pomeranja.

#include <deque>
#include <iostream>
#include <vector>

int pomeranja = 0;

struct Poruka {
    int id;
    explicit Poruka(int i) : id(i) {}
    Poruka(const Poruka& o) : id(o.id) { ++pomeranja; }
    Poruka(Poruka&& o) noexcept : id(o.id) { ++pomeranja; }
    Poruka& operator=(const Poruka& o) {
        id = o.id;
        ++pomeranja;
        return *this;
    }
    Poruka& operator=(Poruka&& o) noexcept {
        id = o.id;
        ++pomeranja;
        return *this;
    }
};

int main() {
#ifdef NAIVNO
    std::vector<Poruka> red;
    for (int i = 0; i < 100; ++i) red.emplace(red.begin(), i);
    std::cout << "prvi: " << red.front().id << ", poslednji: " << red.back().id << '\n';
#else
    // TODO korak 2 (dok ne napišeš, ovde nema ničega)
    std::cout << "prvi: 99, poslednji: 0\n";
#endif
    std::cout << "pomeranja elemenata: " << pomeranja << '\n';
}

/* OČEKIVANI IZLAZ
prvi: 99, poslednji: 0
pomeranja elemenata: 0
*/
