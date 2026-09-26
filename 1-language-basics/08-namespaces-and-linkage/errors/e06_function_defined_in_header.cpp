// STD: c++17
// LINK: support/uses_bad_header.cpp
// EXPECT-GCC: multiple definition of `twice(int)'
// EXPECT-CLANG: multiple definition of `twice(int)'
// POGREŠNO: header sa definicijom obične (ne-inline) funkcije, uključen u dva .cpp.
// Zašto: svaki .cpp koji uključi bad_header.h dobija svoju DEFINICIJU twice().
//   Kompajler svaki TU prevodi posebno i tu nema greške. Linker onda vidi dve
//   definicije istog simbola sa external linkage-om i odbija (ODR, [basic.def.odr]).
//   #pragma once tu ne pomaže: on sprečava dvostruko uključivanje u JEDAN TU.
// Ispravno: u header-u samo deklaracija (int twice(int);), definicija u
//   jednom .cpp. Ili "inline int twice(int x) {...}" u header-u.
#include "support/bad_header.h"

int main() {
    return twice(1);
}
