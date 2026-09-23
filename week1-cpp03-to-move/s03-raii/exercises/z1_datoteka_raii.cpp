// VRSTA: upotreba
//
// Zadatak 1 -- RAII omotač, unique_ptr sa deleter-om i scope guard
// (sekcije 1, 6)
//   ./build.sh week1-cpp03-to-move/s03-raii/exercises/z1_datoteka_raii.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_datoteka_raii.cpp
//
// Brojač zatvaranja je globalan, da se vidi da se svaki fajl zatvori
// tačno jednom. Koristi zatvori(FILE*) umesto direktnog std::fclose.
// Korak 1: class Datoteka -- konstruktor otvori privremeni fajl
//   (std::tmpfile(); ako vrati nullptr, baci std::runtime_error),
//   destruktor pozove zatvori(f_). Kopiju zabrani. Metode
//   void pisi(const std::string&) (std::fputs) i
//   std::string procitajSve() (std::rewind, pa std::fgetc do EOF).
// Korak 2: isto bez pisanja klase: using FilePtr = std::unique_ptr<FILE,
//   Zatvarac>; gde je Zatvarac struct sa
//   void operator()(FILE* f) const { zatvori(f); }.
// Korak 3: ScopeGuard -- template klasa koja čuva lambdu i poziva je u
//   destruktoru. Upotrebi je da ispiše "guard: kraj bloka" pri izlasku iz
//   bloka, čak i kad blok napusti izuzetak.

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

int zatvoreno = 0;
void zatvori(FILE* f) {
    if (f) {
        std::fclose(f);
        ++zatvoreno;
    }
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // {
    //     Datoteka d;
    //     d.pisi("prvi red\n");
    //     d.pisi("drugi red");
    //     std::cout << "sadržaj: [" << d.procitajSve() << "]\n";
    // }
    // std::cout << "zatvoreno: " << zatvoreno << '\n';

    // Korak 2 -- otkomentariši:
    // {
    //     FilePtr f(std::tmpfile());
    //     std::fputs("x", f.get());
    // }
    // std::cout << "zatvoreno: " << zatvoreno << '\n';

    // Korak 3 -- otkomentariši:
    // try {
    //     auto g = ScopeGuard([] { std::cout << "guard: kraj bloka\n"; });
    //     throw std::runtime_error("greška u bloku");
    // } catch (const std::exception& e) {
    //     std::cout << "uhvaćeno: " << e.what() << '\n';
    // }
}

/* OČEKIVANI IZLAZ
sadržaj: [prvi red
drugi red]
zatvoreno: 1
zatvoreno: 2
guard: kraj bloka
uhvaćeno: greška u bloku
*/
