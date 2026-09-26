// Rešenje zadatka ex3_enum_class.

#include <cstdint>
#include <iostream>

// Ako koristiš obični enum i int u API-ju (nije dobro): svaki enum, i svaki
// broj, prolazi kao "komanda", pa pogrešna vrednost stigne do uređaja.
// Treba ovako: enum class -- imena su u svom opsegu (Command::Start), nema
// tihe konverzije, a podloženi tip biraš sam (ovde 1 bajt, kao na žici).
enum class State : std::uint8_t { Off, Running, Fault };
enum class Command : std::uint8_t { Stop, Start, Reset };

const char* name(Command c) {
    switch (c) {   // -Wall upozori ako neki enumerator nije pokriven
        case Command::Stop: return "Stop";
        case Command::Start: return "Start";
        case Command::Reset: return "Reset";
    }
    return "?";
}

void send(Command c) {
    // Broj tražiš eksplicitno, tačno na mestu gde ti treba (protokol).
    auto byte = static_cast<std::uint8_t>(c);
    std::cout << "sending command " << name(c) << ", byte " << int(byte) << '\n';
}

int main() {
    send(Command::Reset);
    send(Command::Start);
    std::cout << "sizeof(Command) = " << sizeof(Command) << '\n';
}
