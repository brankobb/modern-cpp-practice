// Rešenje zadatka z3_enum_class.

#include <cstdint>
#include <iostream>

// Ako koristiš obični enum i int u API-ju (nije dobro): svaki enum, i svaki
// broj, prolazi kao "komanda", pa pogrešna vrednost stigne do uređaja.
// Treba ovako: enum class -- imena su u svom opsegu (Komanda::Kreni), nema
// tihe konverzije, a podloženi tip biraš sam (ovde 1 bajt, kao na žici).
enum class Stanje : std::uint8_t { Iskljuceno, Rad, Greska };
enum class Komanda : std::uint8_t { Stani, Kreni, Resetuj };

const char* ime(Komanda k) {
    switch (k) {   // -Wall upozori ako neki enumerator nije pokriven
        case Komanda::Stani: return "Stani";
        case Komanda::Kreni: return "Kreni";
        case Komanda::Resetuj: return "Resetuj";
    }
    return "?";
}

void posalji(Komanda k) {
    // Broj tražiš eksplicitno, tačno na mestu gde ti treba (protokol).
    auto bajt = static_cast<std::uint8_t>(k);
    std::cout << "šaljem komandu " << ime(k) << ", bajt " << int(bajt) << '\n';
}

int main() {
    posalji(Komanda::Resetuj);
    posalji(Komanda::Kreni);
    std::cout << "sizeof(Komanda) = " << sizeof(Komanda) << '\n';
}
