#include <iostream>
#include <string>
#include <utility>

#include "util.h"

// namespace, linkage i inline -- ISPRAVNI slučajevi. Program ima DVA
// translation unit-a (main.cpp i util.cpp), pa se build-uje sa oba:
//   ./build.sh 1-osnove-jezika/08-namespace-i-linkovanje/main.cpp 1-osnove-jezika/08-namespace-i-linkovanje/util.cpp
// Windows:
//   .\build.ps1 1-osnove-jezika\08-namespace-i-linkovanje\main.cpp 1-osnove-jezika\08-namespace-i-linkovanje\util.cpp
// Sve se kompajlira i radi bez ASan/UBSan prijava (g++ 13 i clang 18,
// C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira ili NE linkuje
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-osnove-jezika/08-namespace-i-linkovanje  proverava oba.

// ---------------------------------------------------------------- 1
namespace geo {
struct Point {
    int x;
    int y;
};
int manhattan(const Point& p) { return (p.x < 0 ? -p.x : p.x) + (p.y < 0 ? -p.y : p.y); }
} // namespace geo

namespace company::project::detail { // C++17: ugnežđeni namespace u jednom redu
int answer() { return 42; }
} // namespace company::project::detail

void s01_namespaceBasics() {
    std::cout << "-- 1. namespace: kvalifikovana imena, ugnežđeni namespace, alias --\n";
    geo::Point p{3, -4};
    std::cout << "  geo::manhattan({3, -4}) = " << geo::manhattan(p) << "\n";
    std::cout << "  company::project::detail::answer() = " << company::project::detail::answer() << "\n";

    namespace cpd = company::project::detail; // alias: kraće ime, isti namespace
    std::cout << "  alias cpd::answer() = " << cpd::answer() << "\n";

    // util je "ponovo otvoren" u util.cpp -- namespace može da se proteže
    // kroz više fajlova i više blokova.
    std::cout << "  util::add(2, 3) = " << util::add(2, 3) << " (definicija je u util.cpp)\n";
}

// ---------------------------------------------------------------- 2
namespace config {
int value = 1;
int timeout = 30;
} // namespace config

void s02_usingDeclarationVsDirective() {
    std::cout << "-- 2. using-deklaracija vs using-direktiva --\n";
    {
        using config::timeout; // using-DEKLARACIJA: uvodi JEDNO ime, kao da je ovde deklarisano
        std::cout << "  using config::timeout;  timeout = " << timeout << "\n";
        // int timeout = 5;  // ne bi se kompajliralo: sukob u istom scope-u (errors/e03)
    }
    {
        using namespace config; // using-DIREKTIVA: sva imena postaju VIDLJIVA, "sa strane"
        std::cout << "  using namespace config;  value = " << value << " timeout = " << timeout << "\n";
        // Direktiva ne deklariše ništa u ovom scope-u. Zato bi "int value = 2;"
        // ovde bila ispravna lokalna promenljiva koja TIHO sakrije config::value
        // (g++ ćuti, clang upozori tek sa -Wshadow). Sa using-deklaracijom bi
        // ista linija bila greška (errors/e03).
    }
    // Direktiva je zgodna u .cpp fajlu, u funkciji. U header-u NIKAD
    // (Core Guidelines SF.7): svako ko uključi header dobija sva ta imena
    // i sukobe (errors/e02, e04).
}

// ---------------------------------------------------------------- 3
namespace geo {
void print(const Point& p) { std::cout << "geo::print(" << p.x << ", " << p.y << ")"; }

struct Buffer {
    std::string data;
};
// Sopstveni swap u ISTOM namespace-u kao tip (EC++ Item 25).
void swap(Buffer& a, Buffer& b) noexcept {
    std::cout << "geo::swap ";
    a.data.swap(b.data);
}
} // namespace geo

void s03_adl() {
    std::cout << "-- 3. ADL: argument-dependent lookup --\n";
    geo::Point p{1, 2};
    std::cout << "  print(p) bez geo:: -> ";
    print(p); // ADL: argument je geo::Point, pa se traži i u namespace-u geo
    std::cout << "\n";

    // std::cout << "x" je poziv operator<<(std::ostream&, const char*).
    // Pronalazi se ADL-om, jer je std::cout iz namespace-a std.
    std::cout << "  operator<< za std::ostream pronađen ADL-om\n";

    // Idiom "using std::swap; swap(a, b);": ADL nađe geo::swap ako postoji,
    // a za tipove bez svog swap-a koristi se std::swap.
    geo::Buffer a{"prvi"}, b{"drugi"};
    int i = 1, j = 2;
    using std::swap;
    std::cout << "  swap(Buffer, Buffer) -> ";
    swap(a, b);
    std::cout << "a=" << a.data << " b=" << b.data << "\n";
    swap(i, j);
    std::cout << "  swap(int, int) -> std::swap, i=" << i << " j=" << j << "\n";
    // Da si napisao std::swap(a, b), geo::swap se NE bi pozvao -- kvalifikovan
    // poziv isključuje ADL, pa bi se uzeo generički std::swap (tri move-a).
}

// ---------------------------------------------------------------- 4
namespace { // internal linkage: vidi se samo u main.cpp
std::string describe() { return "describe() iz main.cpp"; }
} // namespace

// static na nivou namespace-a znači isto što i anonimni namespace (internal
// linkage). Anonimni namespace je moderniji jer radi i za tipove.
static int localHelper() { return 7; }

void s04_linkage() {
    std::cout << "-- 4. linkage: external, internal, extern --\n";
    std::cout << "  main.cpp: " << describe() << "\n";
    std::cout << "  util.cpp: " << util::describeFromUtil() << "  <- isto ime, druga funkcija\n";
    std::cout << "  static localHelper() = " << localHelper() << " (internal linkage)\n";

    // util::callCount je DEFINISAN u util.cpp, a util.h ima samo "extern int callCount;".
    std::cout << "  util::callCount pre = " << util::callCount;
    util::add(1, 1);
    util::add(2, 2);
    std::cout << ", posle dva add() = " << util::callCount << " (ista promenljiva u oba TU-a)\n";
}

// ---------------------------------------------------------------- 5
void s05_inline() {
    std::cout << "-- 5. inline: ista definicija sme u više TU-ova --\n";
    const util::Addresses u = util::addressesSeenByUtil();
    auto same = [](const void* a, const void* b) { return a == b ? "ISTA" : "RAZLIČITA"; };

    std::cout << "  adresa u main.cpp vs util.cpp:\n";
    std::cout << "    inline int sharedVar              -> " << same(&util::sharedVar, u.sharedVar) << "\n";
    std::cout << "    static int perTuVar               -> " << same(&util::perTuVar, u.perTuVar) << "  <- kopija po TU\n";
    std::cout << "    const int perTuConst              -> " << same(&util::perTuConst, u.perTuConst) << "  <- const = internal linkage\n";
    std::cout << "    inline constexpr int sharedConst  -> " << same(&util::sharedConst, u.sharedConst) << "\n";
    std::cout << "    static constexpr Config::maxUsers -> " << same(&util::Config::maxUsers, u.maxUsers) << "  <- implicitno inline (C++17)\n";
    std::cout << "    inline static Config::instances   -> " << same(&util::Config::instances, u.instances) << "\n";

    // inline funkcija: JEDNA funkcija u programu, pa i JEDAN static brojač.
    int t1 = util::nextTicket();
    int t2 = util::ticketsFromUtil();
    int t3 = util::nextTicket();
    std::cout << "  inline nextTicket(): main=" << t1 << " util=" << t2 << " main=" << t3 << "  <- jedan brojač\n";

    // static funkcija u header-u: svaki TU ima svoju funkciju i svoj brojač.
    int l1 = util::nextLocalTicket();
    int l2 = util::localTicketsFromUtil();
    int l3 = util::nextLocalTicket();
    std::cout << "  static nextLocalTicket(): main=" << l1 << " util=" << l2 << " main=" << l3
              << "  <- dva brojača!\n";

    util::Config cfg;
    std::cout << "  Config::limit() definisan u klasi (implicitno inline) = " << cfg.limit() << "\n";
}

// ---------------------------------------------------------------- 6
namespace lib {
inline namespace v2 { // inline namespace: njegova imena su vidljiva i kao lib::...
int api() { return 2; }
} // namespace v2
namespace v1 {
int api() { return 1; }
} // namespace v1
} // namespace lib

void s06_inlineNamespace() {
    std::cout << "-- 6. inline namespace: verzionisanje API-ja --\n";
    std::cout << "  lib::api()=" << lib::api() << " (podrazumevana v2), lib::v1::api()=" << lib::v1::api()
              << ", lib::v2::api()=" << lib::v2::api() << "\n";
    // Tako rade i std::literals (inline namespace) i verzije standardne
    // biblioteke (libstdc++ ima std::__cxx11 za novi std::string).
}

// ---------------------------------------------------------------- 7
// Globalna promenljiva čiju vrednost računa drugi TU. Da je util.cpp imao
// globalni "std::string defaultName = ...", redosled inicijalizacije main.cpp
// i util.cpp ne bi bio određen, pa bi greeting mogao da pročita još
// nekonstruisan string (ub/u01). Funkcija sa static lokalnom promenljivom
// (EC++ Item 4) konstruiše vrednost pri prvom pozivu, pa je ovo uvek ispravno.
const std::string greeting = "Zdravo, " + util::defaultName();

void s07_staticInitOrder() {
    std::cout << "-- 7. redosled inicijalizacije globalnih promenljivih između TU-ova --\n";
    std::cout << "  greeting = \"" << greeting << "\"  <- defaultName() je Meyers singleton\n";
}

int main() {
    s01_namespaceBasics();
    s02_usingDeclarationVsDirective();
    s03_adl();
    s04_linkage();
    s05_inline();
    s06_inlineNamespace();
    s07_staticInitOrder();
}
