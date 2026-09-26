// EXPECT-UB: terminate called after throwing an instance of 'std::runtime_error'
// POGREŠNO: destruktor baca izuzetak.
// Zašto: od C++11 su destruktori implicitno noexcept. Izuzetak koji izađe
//   iz noexcept funkcije poziva std::terminate: catch (...) u main-u ga
//   nikad ne vidi. Ovo nije UB nego zagarantovan prekid programa, a oba
//   kompajlera upozore (g++ -Wterminate, clang -Wexceptions).
//   Pre C++11 je problem bio isti, samo ređi: izuzetak iz destruktora
//   TOKOM unwinding-a drugog izuzetka takođe poziva std::terminate.
// Ispravno (EC++ Item 8): destruktor hvata i beleži grešku; operacija koja
//   može da ne uspe je posebna funkcija (close()) koju pozivalac zove sam
//   (main.cpp, sekcija 5).
#include <cstdio>
#include <stdexcept>

struct Closer {
    ~Closer() { throw std::runtime_error("close failed"); }
};

int main() {
    try {
        Closer c;
    } catch (...) {
        std::puts("caught");
    }
}
