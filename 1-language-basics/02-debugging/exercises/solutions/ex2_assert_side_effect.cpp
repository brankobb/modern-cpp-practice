// Rešenje zadatka ex2_assert_side_effect.

#include <cassert>
#include <iostream>

bool initialized = false;

bool initialize() {
    initialized = true;
    return true;
}

int main() {
    // Ako pišeš assert(initialize()) (nije dobro): sa -DNDEBUG poziv
    // nestane zajedno sa assert-om.
    // Treba ovako: posao van assert-a, u assert-u samo provera.
    // [[maybe_unused]]: sa NDEBUG je ok neiskorišćen, i to je u redu.
    [[maybe_unused]] bool ok = initialize();
    assert(ok && "sensor initialization");
    std::cout << "sensor initialized: " << (initialized ? "yes" : "no") << '\n';
    // Korak 3: ne. assert je za greške u PROGRAMU (pretpostavke koje moraju
    // da važe). Greška iz okoline (nema senzora) se proverava uvek:
    // if (!initialize()) { prijavi grešku / baci izuzetak }.
}
