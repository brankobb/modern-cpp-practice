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
