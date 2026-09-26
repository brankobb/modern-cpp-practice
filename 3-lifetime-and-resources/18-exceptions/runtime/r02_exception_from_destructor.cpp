// EXPECT-RUN: terminate called after throwing an instance of 'std::runtime_error'
// NIJE UB, ali program se prekine: destruktor je od C++11 implicitno
// noexcept, pa izuzetak koji iz njega izleti poziva std::terminate -- i
// catch (...) oko bloka NE pomaže. g++ upozori (-Wterminate), clang
// (-Wexceptions).
// Ispravno: destruktor ne baca (EC++ Item 8). Grešku pri zatvaranju
// zabeleži, ili ponudi posebnu funkciju close() koju pozivalac zove i
// čiji izuzetak može da uhvati (lekcija 21, sekcija 5).
#include <iostream>
#include <stdexcept>
struct File {
    ~File() { throw std::runtime_error("close failed"); }
};
int main() {
    try {
        File f;
    } catch (...) {
        std::cout << "caught\n";   // nikad se ne izvrši
    }
}
