// VRSTA: upotreba
//
// Zadatak 2 -- sopstveni rastući niz: šta std::vector radi za tebe (sekcija 5)
//   ./build.sh 1-osnove-jezika/13-dinamicka-memorija/exercises/z2_dinamicki_niz.cpp
// Rešenje: exercises/solutions/z2_dinamicki_niz.cpp
//
// class DinamickiNiz čuva int-ove u bloku sa new[]: pokazivač podaci_,
// broj elemenata vel_ i kapacitet kap_.
// Korak 1: destruktor (delete[]), i zabrani kopiranje (= delete za copy
//   konstruktor i copy dodelu) -- kopija bi delila isti blok, pa bi ga oba
//   destruktora oslobodila (lekcija 14, ub/u02).
// Korak 2: void dodaj(int x): ako je vel_ == kap_, NOVI kapacitet je
//   kap_ ? 2 * kap_ : 1; alociraj novi blok, prepiši elemente, oslobodi
//   stari, pa upiši x. Redosled je bitan: stari blok se briše tek POSLE
//   kopiranja. Ispiši "rast: <stari> -> <novi>" pri svakom rastu.
// Korak 3: int at(std::size_t i) const (baca std::out_of_range van
//   opsega), std::size_t velicina() const, std::size_t kapacitet() const.

#include <cstddef>
#include <iostream>
#include <stdexcept>

class DinamickiNiz {
public:
    // TODO korak 1, 2, 3
};

int main() {
    // Korak 1-3 -- otkomentariši:
    // DinamickiNiz n;
    // for (int i = 1; i <= 10; ++i) n.dodaj(i);
    // int zbir = 0;
    // for (std::size_t i = 0; i < n.velicina(); ++i) zbir += n.at(i);
    // std::cout << "velicina: " << n.velicina() << ", kapacitet: " << n.kapacitet()
    //           << ", zbir: " << zbir << '\n';
    // try {
    //     n.at(10);
    // } catch (const std::out_of_range&) {
    //     std::cout << "at(10): out_of_range\n";
    // }
}

/* OČEKIVANI IZLAZ
rast: 0 -> 1
rast: 1 -> 2
rast: 2 -> 4
rast: 4 -> 8
rast: 8 -> 16
velicina: 10, kapacitet: 16, zbir: 55
at(10): out_of_range
*/
