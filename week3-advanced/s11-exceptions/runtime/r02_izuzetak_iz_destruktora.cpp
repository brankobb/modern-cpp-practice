// EXPECT-RUN: terminate called after throwing an instance of 'std::runtime_error'
// NIJE UB, ali program se prekine: destruktor je od C++11 implicitno
// noexcept, pa izuzetak koji iz njega izleti poziva std::terminate -- i
// catch (...) oko bloka NE pomaže. g++ upozori (-Wterminate), clang
// (-Wexceptions).
// Ispravno: destruktor ne baca (EC++ Item 8). Grešku pri zatvaranju
// zabeleži, ili ponudi posebnu funkciju zatvori() koju pozivalac zove i
// čiji izuzetak može da uhvati (week1 s03, sekcija 5).
#include <iostream>
#include <stdexcept>
struct Fajl {
    ~Fajl() { throw std::runtime_error("zatvaranje nije uspelo"); }
};
int main() {
    try {
        Fajl f;
    } catch (...) {
        std::cout << "uhvaćeno\n";   // nikad se ne izvrši
    }
}
