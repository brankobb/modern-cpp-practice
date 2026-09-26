// VRSTA: upotreba
//
// Zadatak 1 -- klasa sa invarijantom, delegiranje, this i static (sekcije 1, 2, 5, 6, 9)
//   ./build.sh 2-klase/14-klase-osnove/exercises/z1_racun.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom OČEKIVANI IZLAZ na dnu fajla.
// Rešenje: exercises/solutions/z1_racun.cpp
//
// class Racun: privatni std::string vlasnik_ i long stanje_ (u parama).
// Invarijanta: stanje_ >= 0 u svakom trenutku.
// Korak 1: konstruktor Racun(std::string vlasnik, long pocetno) baca
//   std::invalid_argument ako je pocetno < 0 (objekat sa pokvarenom
//   invarijantom tada ni ne nastaje). Drugi konstruktor,
//   explicit Racun(std::string vlasnik), DELEGIRA prvom sa pocetno = 0.
// Korak 2: Racun& uplati(long iznos) vraća *this, pa se pozivi mogu
//   nizati: r.uplati(100).uplati(50). bool isplati(long iznos) ne dozvoli
//   minus (vrati false). long stanje() const, const std::string& vlasnik() const.
// Korak 3: static int brojZivih() -- koliko Racun objekata trenutno postoji.
//   Brojač je static član (jedan za celu klasu). Uvećaj ga u SVAKOM
//   konstruktoru koji zaista pravi objekat (pazi na delegiranje: ne broji
//   dvaput!), umanji u destruktoru. Kopiju zabrani (= delete) -- račun se
//   ne kopira.

#include <iostream>
#include <stdexcept>
#include <string>

class Racun {
public:
    // TODO korak 1, 2, 3
};

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // Racun a("Ana", 1000);
    // a.uplati(100).uplati(50);
    // bool ok = a.isplati(2000);
    // std::cout << a.vlasnik() << ": " << a.stanje() << ", isplata 2000: " << ok << '\n';
    // try {
    //     Racun los("Loš", -5);
    // } catch (const std::invalid_argument& e) {
    //     std::cout << "odbijeno: " << e.what() << '\n';
    // }

    // Korak 3 -- otkomentariši:
    // std::cout << "živih: " << Racun::brojZivih() << '\n';
    // {
    //     Racun b("Bora");
    //     std::cout << "živih u bloku: " << Racun::brojZivih() << ", Bora: " << b.stanje() << '\n';
    // }
    // std::cout << "živih posle bloka: " << Racun::brojZivih() << '\n';
}

/* OČEKIVANI IZLAZ
Ana: 1150, isplata 2000: 0
odbijeno: početno stanje < 0
živih: 1
živih u bloku: 2, Bora: 0
živih posle bloka: 1
*/
