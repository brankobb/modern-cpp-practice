// EXPECT-GCC: cannot return from a handler of a function-try-block of a constructor
// EXPECT-CLANG: return in the catch of a function try block of a constructor is illegal
// POGREŠNO: handler function-try-block-a konstruktora ne može da "proguta"
// izuzetak: članovi su već uništeni, objekat ne postoji, pa nema šta da se
// vrati pozivaocu. Na kraju handler-a izuzetak se UVEK baca dalje.
// Ispravno: prevedi izuzetak (throw OtherType(...)) ili ga pusti dalje;
// ako objekat mora da postoji i kad nešto ne uspe, grešku reši u telu
// konstruktora (main.cpp, sekcija 7).
#include <stdexcept>
struct Device {
    explicit Device(int n) try : n_(n) {
        if (n < 0) throw std::invalid_argument("n < 0");
    } catch (...) {
        return;
    }
    int n_;
};
int main() { Device u(1); }
