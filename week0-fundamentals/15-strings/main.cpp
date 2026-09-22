#include <charconv>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

// Stringovi: literali, raw stringovi, std::string, string streams i
// korisnički literali -- ISPRAVNI slučajevi. Sve se kompajlira i radi bez
// ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh week0-fundamentals/15-strings  proverava oba.

// ---------------------------------------------------------------- 1
void s01_literals() {
    std::cout << "-- 1. string literali --\n";
    // "abc" je const char[4]: tri znaka + '\0'. Živi ceo program.
    std::cout << "  sizeof(\"abc\")=" << sizeof("abc") << " (3 znaka + '\\0')\n";
    const char* joined = "Zdravo, " "svete"; // susedni literali se spoje PRI KOMPAJLIRANJU
    std::cout << "  \"Zdravo, \" \"svete\" -> " << joined << "  (\"a\" + \"b\" se ne kompajlira, errors/e01)\n";
    std::cout << "  escape: tab[\t] navodnik[\"] backslash[\\]\n";
}

// ---------------------------------------------------------------- 2
void s02_rawStrings() {
    std::cout << "-- 2. raw string literali (C++11) --\n";
    const char* path = R"(C:\Users\ana\novi fajl.txt)";         // backslash nije escape
    const char* regex = R"(\d{3}-\d{4})";                       // regex bez udvostručavanja
    const char* quoted = R"x(poziv f(")") -- sadrži )" )x";      // sopstveni graničnik x kad tekst sadrži )"
    const char* multiline = R"(prvi red
drugi red)";                                                   // novi red je deo stringa
    std::cout << "  " << path << "\n  " << regex << "\n  " << quoted << "\n  " << multiline << "\n";
}

// ---------------------------------------------------------------- 3
void s03_stdString() {
    std::cout << "-- 3. std::string: osnovne operacije --\n";
    std::string s = "Dobar dan";
    s += ", svete";              // dodavanje
    s.insert(0, ">> ");          // umetanje
    s.replace(3, 5, "Lep");      // zameni 5 znakova od pozicije 3
    std::cout << "  s=\"" << s << "\" size=" << s.size() << " front='" << s.front() << "' back='" << s.back() << "'\n";

    std::size_t comma = s.find(',');
    std::string before = s.substr(0, comma);     // substr(pozicija, dužina) pravi NOVI string
    std::cout << "  find(',')=" << comma << " substr(0, " << comma << ")=\"" << before << "\"\n";

    std::size_t missing = s.find('z');
    std::cout << "  find('z')=" << (missing == std::string::npos ? "npos" : "nađeno")
              << " -- uvek poredi sa std::string::npos (to je najveći size_t, ne -1 tipa int)\n";

    try {
        std::cout << "  s.at(100): ";
        std::cout << s.at(100);
    } catch (const std::out_of_range&) {
        std::cout << "std::out_of_range  (s[100] bi bio UB, ub/u03)\n";
    }

    std::string a = "jabuka";
    std::string b = "kruška";
    std::cout << std::boolalpha << "  a < b: " << (a < b) << " (leksikografski, po bajtovima), a == \"jabuka\": " << (a == "jabuka")
              << std::noboolalpha << "\n";
}

// ---------------------------------------------------------------- 4
void s04_bytesNotCharacters() {
    std::cout << "-- 4. size() broji bajtove, ne slova --\n";
    std::string ascii = "casa";
    std::string utf8 = "čaša"; // izvorni fajl je UTF-8: č i š su po 2 bajta
    std::cout << "  \"casa\".size()=" << ascii.size() << "  \"čaša\".size()=" << utf8.size()
              << "  <- std::string čuva bajtove; broj slova zahteva UTF-8 dekodiranje\n";
    std::string s;
    s = 65; // kompajlira se: dodela jednog char-a ('A'). Tiha zamka.
    std::cout << "  std::string s; s = 65; -> \"" << s << "\"\n";
}

// ---------------------------------------------------------------- 5
void s05_conversions() {
    std::cout << "-- 5. brojevi <-> tekst --\n";
    std::cout << "  to_string(42)=\"" << std::to_string(42) << "\" to_string(1.5)=\"" << std::to_string(1.5) << "\"\n";

    std::size_t used = 0;
    int partial = std::stoi("42abc", &used); // čita dok može; used kaže koliko
    std::cout << "  stoi(\"42abc\")=" << partial << " (pročitano " << used << " znaka -- ostatak tiho ignorisan)\n";
    try {
        std::stoi("abc");
    } catch (const std::invalid_argument&) {
        std::cout << "  stoi(\"abc\") -> std::invalid_argument\n";
    }
    try {
        std::stoi("99999999999");
    } catch (const std::out_of_range&) {
        std::cout << "  stoi(\"99999999999\") -> std::out_of_range\n";
    }

    // C++17 from_chars: bez izuzetaka, bez locale-a, bez alokacije; greška u rezultatu.
    std::string_view input = "123x";
    int value = 0;
    auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
    std::cout << "  from_chars(\"123x\"): value=" << value << " ok=" << (ec == std::errc{} ? "da" : "ne")
              << " ostatak=\"" << std::string_view(ptr, static_cast<std::size_t>(input.data() + input.size() - ptr)) << "\"\n";
}

// ---------------------------------------------------------------- 6
void s06_cStrAndCapacity() {
    std::cout << "-- 6. c_str() i kapacitet --\n";
    std::string name = "Ana";
    const char* c = name.c_str(); // važi dok se name ne promeni ili ne nestane (ub/u01, ub/u02)
    std::cout << "  c_str()=\"" << c << "\" (za C API: fopen, printf)\n";

    std::string built;
    built.reserve(100); // jedna alokacija umesto više realokacija u petlji
    for (int i = 0; i < 10; ++i) built += std::to_string(i);
    std::cout << "  posle reserve(100) i 10 dodavanja: size=" << built.size() << " capacity>=" << (built.capacity() >= 100 ? "100" : "?")
              << "\n  kratak string (SSO) ne alocira: week1 s04, sekcija 8\n";
}

// ---------------------------------------------------------------- 7
void s07_stringStreams() {
    std::cout << "-- 7. string streams (kurs 88) --\n";
    std::ostringstream out; // pravljenje formatiranog teksta
    out << "cena=" << std::fixed << std::setprecision(2) << 3.14159 << " kolicina=" << std::setw(4) << 7;
    std::cout << "  ostringstream: \"" << out.str() << "\"\n";

    std::istringstream in("10 20 x 30"); // čitanje brojeva iz teksta
    int a = 0, b = 0, c = -1;
    in >> a >> b >> c; // "x" nije broj: c postane 0, stream u fail stanju
    std::cout << "  istringstream \"10 20 x 30\": a=" << a << " b=" << b << " c=" << c << " fail=" << in.fail() << "\n";

    std::istringstream csv("Ana,23,Beograd");
    std::vector<std::string> fields;
    for (std::string field; std::getline(csv, field, ',');) fields.push_back(field); // getline sa graničnikom
    std::cout << "  getline sa ',': " << fields.size() << " polja: " << fields[0] << " | " << fields[1] << " | " << fields[2] << "\n";

    std::istringstream reused("5");
    int x = 0;
    reused >> x >> x; // drugo čitanje ne uspe: kraj -> fail + eof
    reused.str("7");  // novi sadržaj, ali stanje greške OSTAJE
    int y = 0;
    reused >> y;
    std::cout << "  ponovna upotreba bez clear(): y=" << y;
    reused.clear();   // obriši fail/eof
    reused.str("7");
    reused >> y;
    std::cout << "; posle clear(): y=" << y << "\n";
}

// ---------------------------------------------------------------- 8
struct Meters {
    long double value;
};

// Korisnički literal: sufiks MORA da počinje sa _ (ostali su rezervisani za standard).
constexpr Meters operator""_m(long double v) { return Meters{v}; }
constexpr Meters operator""_km(long double v) { return Meters{v * 1000}; }
constexpr Meters operator""_km(unsigned long long v) { return Meters{static_cast<long double>(v) * 1000}; }

void s08_userDefinedLiterals() {
    std::cout << "-- 8. korisnički literali (kurs 90) --\n";
    using namespace std::string_literals;       // "..."s
    using namespace std::string_view_literals;  // "..."sv
    using namespace std::chrono_literals;       // 100ms, 2s, 5min

    auto text = "tekst"s;               // std::string, ne const char*
    auto withNull = "a\0b"s;            // dužina iz literala: 3 (std::string("a\0b") bi imao 1)
    auto view = "pogled"sv;             // std::string_view
    auto timeout = 1500ms;              // std::chrono::milliseconds
    std::cout << "  \"tekst\"s.size()=" << text.size() << "  \"a\\0b\"s.size()=" << withNull.size()
              << " std::string(\"a\\0b\").size()=" << std::string("a\0b").size() << "  \"pogled\"sv.size()=" << view.size() << "\n";
    std::cout << "  1500ms = " << std::chrono::duration_cast<std::chrono::seconds>(timeout).count() << " s (odsečeno) = "
              << timeout.count() << " ms\n";

    constexpr Meters run = 2.5_km;       // long double verzija
    constexpr Meters lap = 400.0_m;
    constexpr Meters walk = 3_km;        // unsigned long long verzija
    std::cout << "  2.5_km=" << static_cast<double>(run.value) << " m, 400.0_m=" << static_cast<double>(lap.value)
              << " m, 3_km=" << static_cast<double>(walk.value) << " m (sve pri kompajliranju, constexpr)\n";
}

int main() {
    s01_literals();
    s02_rawStrings();
    s03_stdString();
    s04_bytesNotCharacters();
    s05_conversions();
    s06_cStrAndCapacity();
    s07_stringStreams();
    s08_userDefinedLiterals();
}
