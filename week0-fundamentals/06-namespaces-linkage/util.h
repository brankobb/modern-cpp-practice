// util.h -- uključuju ga I main.cpp I util.cpp, dakle DVA translation unit-a
// (TU = .cpp fajl posle preprocesora). Sve što je ovde napisano postoji u
// oba TU-a, pa se vidi šta linker spaja u jedno, a šta ostaje po kopija u
// svakom TU-u.
//
// #pragma once: header se u jednom TU-u uključi najviše jednom. Isto radi
// klasičan include guard (#ifndef UTIL_H / #define UTIL_H / #endif), koji je
// standardan. #pragma once nije u standardu, ali ga podržavaju svi glavni
// kompajleri. Guard štiti od dupliranja u JEDNOM TU-u, a ne od toga da
// linker vidi istu definiciju iz DVA TU-a (errors/e06).
#pragma once

#include <string>

namespace util {

// ---- DEKLARACIJE: definicije su u util.cpp (external linkage) -----------
// Ako ovde napišeš i telo "int add(int a, int b) { ... }", svaki TU dobija
// svoju definiciju i linker javi "multiple definition" (errors/e06).
int add(int a, int b);

// Promenljiva: "extern" bez inicijalizatora je samo DEKLARACIJA.
// Bez extern bi ovo bila definicija u svakom TU-u (errors/e09).
extern int callCount;

// Meyers singleton (EC++ Item 4): funkcija sa static lokalnom promenljivom.
// Inicijalizuje se pri prvom pozivu, pa ne zavisi od redosleda TU-ova (ub/u01).
const std::string& defaultName();

// Poziva util.cpp-ov describe() iz anonimnog namespace-a (sekcija 4).
std::string describeFromUtil();

// ---- DEFINICIJE u header-u: moraju biti inline --------------------------
// inline = "ova definicija sme da postoji u više TU-ova, linker zadrži
// jednu". Svi TU-ovi vide ISTU funkciju, pa i isti static brojač.
inline int nextTicket() {
    static int n = 0;
    return ++n;
}

// NE RADI OVAKO: static funkcija u header-u. Svaki TU dobija SVOJU kopiju
// funkcije i SVOJ brojač. Kompajlira se, ali "globalni" brojač nije globalan.
static int nextLocalTicket() {
    static int n = 0;
    return ++n;
}

inline int sharedVar = 0;             // C++17: JEDNA promenljiva u programu
static int perTuVar = 0;              // NE RADI OVAKO: svaki TU svoju kopiju
const int perTuConst = 42;            // const na nivou namespace-a -> internal linkage, kopija po TU
inline constexpr int sharedConst = 42; // jedna konstanta za ceo program

// Funkcije i static constexpr članovi definisani u klasi su implicitno inline.
struct Config {
    static constexpr int maxUsers = 100; // C++17: implicitno inline
    inline static int instances = 0;     // C++17: definicija u klasi, bez .cpp
    int limit() const { return maxUsers; } // implicitno inline
};

// Adrese kako ih vidi util.cpp, da main.cpp može da uporedi sa svojim.
struct Addresses {
    const void* sharedVar;
    const void* perTuVar;
    const void* perTuConst;
    const void* sharedConst;
    const void* maxUsers;
    const void* instances;
};
Addresses addressesSeenByUtil();
int ticketsFromUtil();       // poziva nextTicket() iz util.cpp
int localTicketsFromUtil();  // poziva nextLocalTicket() iz util.cpp

} // namespace util
