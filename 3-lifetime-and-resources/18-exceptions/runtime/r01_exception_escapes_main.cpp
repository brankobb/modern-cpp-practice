// EXPECT-RUN: terminate called after throwing an instance of 'std::runtime_error'
// NIJE UB, ali program se prekine: izuzetak koji niko ne uhvati poziva
// std::terminate ([except.handle]). Da li se pre toga lokalni objekti
// uništavaju (stack unwinding) -- nije određeno; sa g++ i clang se NE
// uništavaju (test: destruktor lokalnog objekta ne ispiše ništa), pa
// destruktori (npr. zatvaranje fajla) ne rade.
// Poruka "terminate called after throwing..." je od libstdc++; libc++
// (MSYS2 clang64) ima drugačiji tekst.
// Ispravno: try/catch u main-u oko celog posla, bar za std::exception.
#include <iostream>
#include <stdexcept>
int main() {
    std::cout << "start\n";
    throw std::runtime_error("nobody catches it");
}
