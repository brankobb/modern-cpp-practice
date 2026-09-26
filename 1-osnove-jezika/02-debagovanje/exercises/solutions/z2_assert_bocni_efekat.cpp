// Rešenje zadatka z2_assert_bocni_efekat.

#include <cassert>
#include <iostream>

bool inicijalizovan = false;

bool inicijalizuj() {
    inicijalizovan = true;
    return true;
}

int main() {
    // Ako pišeš assert(inicijalizuj()) (nije dobro): sa -DNDEBUG poziv
    // nestane zajedno sa assert-om.
    // Treba ovako: posao van assert-a, u assert-u samo provera.
    // [[maybe_unused]]: sa NDEBUG je ok neiskorišćen, i to je u redu.
    [[maybe_unused]] bool ok = inicijalizuj();
    assert(ok && "inicijalizacija senzora");
    std::cout << "senzor inicijalizovan: " << (inicijalizovan ? "da" : "ne") << '\n';
    // Korak 3: ne. assert je za greške u PROGRAMU (pretpostavke koje moraju
    // da važe). Greška iz okoline (nema senzora) se proverava uvek:
    // if (!inicijalizuj()) { prijavi grešku / baci izuzetak }.
}
