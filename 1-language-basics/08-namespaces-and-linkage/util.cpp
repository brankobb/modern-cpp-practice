// util.cpp -- drugi translation unit. Build zajedno sa main.cpp:
//   ./build.sh 1-language-basics/08-namespaces-and-linkage/main.cpp 1-language-basics/08-namespaces-and-linkage/util.cpp
#include "util.h"

namespace { // anonimni namespace: sve ovde ima INTERNAL linkage
// main.cpp ima SVOJ describe() u svom anonimnom namespace-u. Dve različite
// funkcije istog imena, bez sukoba, jer nijedna nije vidljiva linkeru.
std::string describe() { return "describe() from util.cpp"; }
} // namespace

namespace util { // isti namespace kao u util.h -- namespace se sme "ponovo otvoriti"

int callCount = 0; // DEFINICIJA (jedna u programu); util.h ima samo extern deklaraciju

int add(int a, int b) {
    ++callCount;
    return a + b;
}

const std::string& defaultName() {
    static const std::string name = "world"; // inicijalizuje se pri PRVOM pozivu
    return name;
}

std::string describeFromUtil() { return describe(); }

Addresses addressesSeenByUtil() {
    return {&sharedVar, &perTuVar, &perTuConst, &sharedConst, &Config::maxUsers, &Config::instances};
}

int ticketsFromUtil() { return nextTicket(); }
int localTicketsFromUtil() { return nextLocalTicket(); }

} // namespace util
