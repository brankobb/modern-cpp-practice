// STD: c++17
// LINK: support/uses_counter_header.cpp
// EXPECT-GCC: multiple definition of `counter'
// EXPECT-CLANG: multiple definition of `counter'
// POGREŠNO: "int counter = 0;" u header-u koji uključuju dva .cpp.
// Zašto: to je DEFINICIJA promenljive sa external linkage-om, pa je svaki TU
//   ima po jednu. Isto kao e06, samo za promenljivu.
// Ispravno (izaberi jedno):
//   - C++17: "inline int counter = 0;" u header-u -- jedna promenljiva;
//   - "extern int counter;" u header-u + "int counter = 0;" u jednom .cpp;
//   - ako je const/constexpr, već ima internal linkage i ovo se ne dešava
//     (ali svaki TU ima svoju kopiju; "inline constexpr" daje jednu).
#include "support/counter_header.h"

int main() {
    return counter;
}
