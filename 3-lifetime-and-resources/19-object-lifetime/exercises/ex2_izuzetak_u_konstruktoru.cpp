// KIND: why
// DEMO-UB: NAIVNO detected memory leaks
//
// Zadatak 2 -- zašto destruktor ne čisti za konstruktorom koji je bacio
// (sekcija 4)
// Rešenje: exercises/solutions/ex2_izuzetak_u_konstruktoru.cpp
//
// Senzor u konstruktoru zauzme dva bafera. Drugi korak (kalibracija) baci
// izuzetak.
//
// Korak 1: pokreni naivnu verziju:
//     ./build.sh 3-lifetime-and-resources/19-object-lifetime/exercises/ex2_izuzetak_u_konstruktoru.cpp -DNAIVNO
//   Na izlazu nema "~Senzor", a LeakSanitizer prijavi curenje oba bafera.
//   Objekat počinje da živi tek kad se konstruktor ZAVRŠI; ovaj se nije
//   završio, pa destruktor koji bi uradio delete[] ne postoji za njega.
//   (Red "uhvaćeno" može da se ne vidi ako izlaz preusmeriš u fajl ili
//   pipe -- LeakSanitizer završi program pre pražnjenja bafera.)
// Korak 2: u #else grani napiši Senzor tako da bafere drže ČLANOVI sa
//   sopstvenim destruktorom: std::vector<int> ili std::unique_ptr<int[]>.
//   Već napravljeni članovi SE uništavaju i kad konstruktor baci -- pa
//   nema curenja, a Senzor više ne treba ni svoj destruktor.
// Korak 3: dodaj u Senzor član Trag (kao u ex1) pre bafera i proveri u
//   izlazu da se njegov destruktor pozove iako konstruktor Senzor-a baci.

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

void kalibrisi(bool uspeh) {
    if (!uspeh) throw std::runtime_error("kalibracija nije uspela");
}

#ifdef NAIVNO
class Senzor {
public:
    explicit Senzor(bool uspeh) {
        sirovi_ = new int[64];
        filtrirani_ = new int[64];
        kalibrisi(uspeh);            // baca: sirovi_ i filtrirani_ ostaju zauzeti
    }
    ~Senzor() {
        std::cout << "~Senzor\n";
        delete[] filtrirani_;
        delete[] sirovi_;
    }
    Senzor(const Senzor&) = delete;
    Senzor& operator=(const Senzor&) = delete;

private:
    int* sirovi_;
    int* filtrirani_;
};

int main() {
    try {
        Senzor s(false);
    } catch (const std::exception& e) {
        std::cout << "uhvaćeno: " << e.what() << '\n';
    }
}
#else
// TODO korak 2 i 3

int main() {
    // Korak 2 i 3 -- otkomentariši:
    // try {
    //     Senzor s(false);
    // } catch (const std::exception& e) {
    //     std::cout << "\nuhvaćeno: " << e.what() << '\n';
    // }
    // {
    //     Senzor ok(true);
    //     std::cout << "\nok: " << ok.velicina() << " elemenata\n";
    // }
    // std::cout << '\n';
}
#endif

/* EXPECTED OUTPUT
 trag() ~trag()
uhvaćeno: kalibracija nije uspela
 trag()
ok: 64 elemenata
 ~trag()
*/
