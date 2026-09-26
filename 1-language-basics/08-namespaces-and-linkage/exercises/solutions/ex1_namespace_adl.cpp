// Rešenje zadatka ex1_namespace_adl.

#include <iostream>

// Korak 3: anonimni namespace = internal linkage. U drugom .cpp fajlu
// isto ime "counter" bila bi druga promenljiva, bez sukoba pri linkovanju.
namespace {
int counter = 0;
}

int initializationCount() { return counter; }

// Korak 1: C++17 ugnežđeni zapis.
namespace company::drivers {

struct Uart {
    int id;
    bool ready;
};

inline constexpr int maxUart = 4;   // inline: sme u header, jedna definicija u programu

void initialize(Uart& u) {
    u.ready = true;
    ++counter;
}

// Korak 2: operator uz tip, u istom namespace-u -> ADL ga nađe iz main().
std::ostream& operator<<(std::ostream& os, const Uart& u) {
    return os << "uart#" << u.id << (u.ready ? " ready" : " off");
}

}  // namespace company::drivers

namespace drv = company::drivers;

// Korak 3: inline namespace -- njegova imena su vidljiva i kao company::ime.
namespace company {
namespace v1 {
inline const char* version() { return "v1"; }
}
inline namespace v2 {
inline const char* version() { return "v2"; }
}
}  // namespace company

int main() {
    drv::Uart a{1, false}, b{2, false}, c{3, false};
    initialize(a);
    initialize(b);
    std::cout << a << '\n' << b << '\n' << c << '\n';

    std::cout << "initialized: " << initializationCount() << " of "
              << drv::maxUart << '\n';
    std::cout << "version: " << company::version() << ", old: "
              << company::v1::version() << '\n';
}
