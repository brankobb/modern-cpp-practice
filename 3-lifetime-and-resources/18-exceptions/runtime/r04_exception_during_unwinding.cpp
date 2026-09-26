// EXPECT-RUN: terminate called after throwing an instance of 'std::runtime_error'
// NIJE UB, ali program se prekine: dok stack unwinding traje zbog PRVOG
// izuzetka, destruktor (ovde eksplicitno noexcept(false)) baci DRUGI. Dva
// aktivna izuzetka u isto vreme -> std::terminate ([except.terminate]).
// Poruka pokazuje drugi izuzetak ("from destructor").
// Ispravno: destruktor ne baca. noexcept(false) na destruktoru je
// gotovo uvek greška.
#include <iostream>
#include <stdexcept>
struct Valve {
    ~Valve() noexcept(false) { throw std::runtime_error("from destructor"); }
};
int main() {
    try {
        Valve v;
        throw std::logic_error("first");
    } catch (...) {
        std::cout << "caught\n";   // nikad se ne izvrši
    }
}
