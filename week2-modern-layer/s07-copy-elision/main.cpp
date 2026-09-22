#include <iostream>
#include <utility>

// Vežba: klasa sa logovanjem u svim specijalnim članovima. Napiši dve
// funkcije koje vraćaju lokalni objekat:
//   Logged makeA() { Logged x; return x; }             // NRVO očekivan
//   Logged makeB() { Logged x; return std::move(x); }  // NRVO onemogućen!
// Uporedi broj poziva ctor/copy/move za obe. Probaj sa -O0 i sa -O2
// (NRVO nije garantovan standardom, ali C++17 garantuje eliziju za
// "return Logged();" direktno).

struct Logged {
    Logged() { std::cout << "ctor\n"; }
    Logged(const Logged&) { std::cout << "copy ctor\n"; }
    Logged(Logged&&) noexcept { std::cout << "move ctor\n"; }
    ~Logged() { std::cout << "dtor\n"; }
};

Logged makeA() {
    Logged x;
    return x; // NRVO kandidat
}

// Ako napišeš return std::move(x); na LOKALNOJ promenljivoj (NIJE DOBRO,
// čest anti-pattern) jer std::move(x) je izraz tipa Logged&& (rvalue
// referenca), NE sam objekat x -- kompajler onda MORA da pozove move ctor
// da napravi povratnu vrednost od te reference, umesto da primeni NRVO i
// izgradi x DIREKTNO na mestu poziva.
// Treba da napišeš samo return x; (bez std::move) kad vraćaš lokalnu
// promenljivu -- kompajler automatski tretira ovo kao "move ili elide"
// kandidata, dobijaš NAJBOLJE od oba bez tvoje intervencije.
// Možeš i koristiti std::move kad vraćaš NEŠTO ŠTO NIJE lokalna
// promenljiva iz OVE funkcije (npr. parametar primljen po vrednosti koji
// vraćaš) -- tu NRVO ionako ne važi, pa std::move ima smisla.
Logged makeB() {
    Logged x;
    return std::move(x); // šteti NRVO-u
}

int main() {
    std::cout << "-- makeA --\n";
    Logged a = makeA();
    std::cout << "-- makeB --\n";
    Logged b = makeB();
    (void)a;
    (void)b;
}
