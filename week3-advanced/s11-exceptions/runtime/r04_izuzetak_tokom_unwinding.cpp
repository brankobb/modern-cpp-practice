// EXPECT-RUN: terminate called after throwing an instance of 'std::runtime_error'
// NIJE UB, ali program se prekine: dok stack unwinding traje zbog PRVOG
// izuzetka, destruktor (ovde eksplicitno noexcept(false)) baci DRUGI. Dva
// aktivna izuzetka u isto vreme -> std::terminate ([except.terminate]).
// Poruka pokazuje drugi izuzetak ("iz destruktora").
// Ispravno: destruktor ne baca. noexcept(false) na destruktoru je
// gotovo uvek greška.
#include <iostream>
#include <stdexcept>
struct Ventil {
    ~Ventil() noexcept(false) { throw std::runtime_error("iz destruktora"); }
};
int main() {
    try {
        Ventil v;
        throw std::logic_error("prvi");
    } catch (...) {
        std::cout << "uhvaćeno\n";   // nikad se ne izvrši
    }
}
