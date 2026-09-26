// Rešenje zadatka ex1_namespace_adl.

#include <iostream>

// Korak 3: anonimni namespace = internal linkage. U drugom .cpp fajlu
// isto ime "brojac" bila bi druga promenljiva, bez sukoba pri linkovanju.
namespace {
int brojac = 0;
}

int brojInicijalizacija() { return brojac; }

// Korak 1: C++17 ugnežđeni zapis.
namespace firma::drajveri {

struct Uart {
    int id;
    bool spreman;
};

inline constexpr int maxUart = 4;   // inline: sme u header, jedna definicija u programu

void inicijalizuj(Uart& u) {
    u.spreman = true;
    ++brojac;
}

// Korak 2: operator uz tip, u istom namespace-u -> ADL ga nađe iz main().
std::ostream& operator<<(std::ostream& os, const Uart& u) {
    return os << "uart#" << u.id << (u.spreman ? " spreman" : " ugašen");
}

}  // namespace firma::drajveri

namespace drv = firma::drajveri;

// Korak 3: inline namespace -- njegova imena su vidljiva i kao firma::ime.
namespace firma {
namespace v1 {
inline const char* verzija() { return "v1"; }
}
inline namespace v2 {
inline const char* verzija() { return "v2"; }
}
}  // namespace firma

int main() {
    drv::Uart a{1, false}, b{2, false}, c{3, false};
    inicijalizuj(a);
    inicijalizuj(b);
    std::cout << a << '\n' << b << '\n' << c << '\n';

    std::cout << "inicijalizovano: " << brojInicijalizacija() << " od "
              << drv::maxUart << '\n';
    std::cout << "verzija: " << firma::verzija() << ", stara: "
              << firma::v1::verzija() << '\n';
}
