// KIND: usage
//
// Zadatak 1 -- RAII omotač, unique_ptr sa deleter-om i scope guard
// (sekcije 1, 6)
//   ./build.sh 3-lifetime-and-resources/21-raii/exercises/ex1_file_raii.cpp
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_file_raii.cpp
//
// Brojač zatvaranja je globalan, da se vidi da se svaki fajl zatvori
// tačno jednom. Koristi closeFile(FILE*) umesto direktnog std::fclose.
// Korak 1: class TempFile -- konstruktor otvori privremeni fajl
//   (std::tmpfile(); ako vrati nullptr, baci std::runtime_error),
//   destruktor pozove closeFile(f_). Kopiju zabrani. Metode
//   void write(const std::string&) (std::fputs) i
//   std::string readAll() (std::rewind, pa std::fgetc do EOF).
// Korak 2: isto bez pisanja klase: using FilePtr = std::unique_ptr<FILE,
//   Closer>; gde je Closer struct sa
//   void operator()(FILE* f) const { closeFile(f); }.
// Korak 3: ScopeGuard -- template klasa koja čuva lambdu i poziva je u
//   destruktoru. Upotrebi je da ispiše "guard: end of block" pri izlasku iz
//   bloka, čak i kad blok napusti izuzetak.

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

int closedCount = 0;
void closeFile(FILE* f) {
    if (f) {
        std::fclose(f);
        ++closedCount;
    }
}

// TODO korak 1, 2, 3

int main() {
    // Korak 1 -- otkomentariši:
    // {
    //     TempFile file;
    //     file.write("first line\n");
    //     file.write("second line");
    //     std::cout << "contents: [" << file.readAll() << "]\n";
    // }
    // std::cout << "closed: " << closedCount << '\n';

    // Korak 2 -- otkomentariši:
    // {
    //     FilePtr f(std::tmpfile());
    //     std::fputs("x", f.get());
    // }
    // std::cout << "closed: " << closedCount << '\n';

    // Korak 3 -- otkomentariši:
    // try {
    //     auto g = ScopeGuard([] { std::cout << "guard: end of block\n"; });
    //     throw std::runtime_error("error in block");
    // } catch (const std::exception& e) {
    //     std::cout << "caught: " << e.what() << '\n';
    // }
}

/* EXPECTED OUTPUT
contents: [first line
second line]
closed: 1
closed: 2
guard: end of block
caught: error in block
*/
