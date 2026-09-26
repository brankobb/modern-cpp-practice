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
// ./check_cases.sh 1-language-basics/06-strings  proverava oba.

// ---------------------------------------------------------------- 1
void s01_literals() {
    std::cout << "-- 1. string literals --\n";
    // "abc" je const char[4]: tri znaka + '\0'. Živi ceo program.
    std::cout << "  sizeof(\"abc\")=" << sizeof("abc") << " (3 characters + '\\0')\n";
    const char* joined = "Hello, " "world"; // susedni literali se spoje PRI KOMPAJLIRANJU
    std::cout << "  \"Hello, \" \"world\" -> " << joined << "  (\"a\" + \"b\" does not compile, errors/e01)\n";
    std::cout << "  escape: tab[\t] quote[\"] backslash[\\]\n";
}

// ---------------------------------------------------------------- 2
void s02_rawStrings() {
    std::cout << "-- 2. raw string literals (C++11) --\n";
    const char* path = R"(C:\Users\ann\new file.txt)";         // backslash nije escape
    const char* regex = R"(\d{3}-\d{4})";                       // regex bez udvostručavanja
    const char* quoted = R"x(call f(")") -- contains )" )x";      // sopstveni graničnik x kad tekst sadrži )"
    const char* multiline = R"(first line
second line)";                                                   // novi red je deo stringa
    std::cout << "  " << path << "\n  " << regex << "\n  " << quoted << "\n  " << multiline << "\n";
}

// ---------------------------------------------------------------- 3
void s03_stdString() {
    std::cout << "-- 3. std::string: basic operations --\n";
    std::string s = "Good day";
    s += ", world";              // dodavanje
    s.insert(0, ">> ");          // umetanje
    s.replace(3, 4, "Nice");     // zameni 4 znaka od pozicije 3
    std::cout << "  s=\"" << s << "\" size=" << s.size() << " front='" << s.front() << "' back='" << s.back() << "'\n";

    std::size_t comma = s.find(',');
    std::string before = s.substr(0, comma);     // substr(pozicija, dužina) pravi NOVI string
    std::cout << "  find(',')=" << comma << " substr(0, " << comma << ")=\"" << before << "\"\n";

    std::size_t missing = s.find('z');
    std::cout << "  find('z')=" << (missing == std::string::npos ? "npos" : "found")
              << " -- always compare with std::string::npos (the largest size_t, not an int -1)\n";

    try {
        std::cout << "  s.at(100): ";
        std::cout << s.at(100);
    } catch (const std::out_of_range&) {
        std::cout << "std::out_of_range  (s[100] would be UB, ub/u03)\n";
    }

    std::string a = "apple";
    std::string b = "pear";
    std::cout << std::boolalpha << "  a < b: " << (a < b) << " (lexicographic, byte by byte), a == \"apple\": " << (a == "apple")
              << std::noboolalpha << "\n";
}

// ---------------------------------------------------------------- 4
void s04_bytesNotCharacters() {
    std::cout << "-- 4. size() counts bytes, not letters --\n";
    std::string ascii = "cafe";
    std::string utf8 = "café"; // izvorni fajl je UTF-8: é je 2 bajta
    std::cout << "  \"cafe\".size()=" << ascii.size() << "  \"café\".size()=" << utf8.size()
              << "  <- std::string stores bytes; counting letters needs UTF-8 decoding\n";
    std::string s;
    s = 65; // kompajlira se: dodela jednog char-a ('A'). Tiha zamka.
    std::cout << "  std::string s; s = 65; -> \"" << s << "\"\n";
}

// ---------------------------------------------------------------- 5
void s05_conversions() {
    std::cout << "-- 5. numbers <-> text --\n";
    std::cout << "  to_string(42)=\"" << std::to_string(42) << "\" to_string(1.5)=\"" << std::to_string(1.5) << "\"\n";

    std::size_t used = 0;
    int partial = std::stoi("42abc", &used); // čita dok može; used kaže koliko
    std::cout << "  stoi(\"42abc\")=" << partial << " (read " << used << " characters -- the rest silently ignored)\n";
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
    std::cout << "  from_chars(\"123x\"): value=" << value << " ok=" << (ec == std::errc{} ? "yes" : "no")
              << " rest=\"" << std::string_view(ptr, static_cast<std::size_t>(input.data() + input.size() - ptr)) << "\"\n";
}

// ---------------------------------------------------------------- 6
void s06_cStrAndCapacity() {
    std::cout << "-- 6. c_str() and capacity --\n";
    std::string name = "Ann";
    const char* c = name.c_str(); // važi dok se name ne promeni ili ne nestane (ub/u01, ub/u02)
    std::cout << "  c_str()=\"" << c << "\" (for a C API: fopen, printf)\n";

    std::string built;
    built.reserve(100); // jedna alokacija umesto više realokacija u petlji
    for (int i = 0; i < 10; ++i) built += std::to_string(i);
    std::cout << "  after reserve(100) and 10 appends: size=" << built.size() << " capacity>=" << (built.capacity() >= 100 ? "100" : "?")
              << "\n  a short string (SSO) does not allocate: lesson 22, section 8\n";
}

// ---------------------------------------------------------------- 7
void s07_stringStreams() {
    std::cout << "-- 7. string streams (course 88) --\n";
    std::ostringstream out; // pravljenje formatiranog teksta
    out << "price=" << std::fixed << std::setprecision(2) << 3.14159 << " quantity=" << std::setw(4) << 7;
    std::cout << "  ostringstream: \"" << out.str() << "\"\n";

    std::istringstream in("10 20 x 30"); // čitanje brojeva iz teksta
    int a = 0, b = 0, c = -1;
    in >> a >> b >> c; // "x" nije broj: c postane 0, stream u fail stanju
    std::cout << "  istringstream \"10 20 x 30\": a=" << a << " b=" << b << " c=" << c << " fail=" << in.fail() << "\n";

    std::istringstream csv("Ann,23,Belgrade");
    std::vector<std::string> fields;
    for (std::string field; std::getline(csv, field, ',');) fields.push_back(field); // getline sa graničnikom
    std::cout << "  getline with ',': " << fields.size() << " fields: " << fields[0] << " | " << fields[1] << " | " << fields[2] << "\n";

    std::istringstream reused("5");
    int x = 0;
    reused >> x >> x; // drugo čitanje ne uspe: kraj -> fail + eof
    reused.str("7");  // novi sadržaj, ali stanje greške OSTAJE
    int y = 0;
    reused >> y;
    std::cout << "  reuse without clear(): y=" << y;
    reused.clear();   // obriši fail/eof
    reused.str("7");
    reused >> y;
    std::cout << "; after clear(): y=" << y << "\n";
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
    std::cout << "-- 8. user-defined literals (course 90) --\n";
    using namespace std::string_literals;       // "..."s
    using namespace std::string_view_literals;  // "..."sv
    using namespace std::chrono_literals;       // 100ms, 2s, 5min

    auto text = "text"s;               // std::string, ne const char*
    auto withNull = "a\0b"s;            // dužina iz literala: 3 (std::string("a\0b") bi imao 1)
    auto view = "view"sv;             // std::string_view
    auto timeout = 1500ms;              // std::chrono::milliseconds
    std::cout << "  \"text\"s.size()=" << text.size() << "  \"a\\0b\"s.size()=" << withNull.size()
              << " std::string(\"a\\0b\").size()=" << std::string("a\0b").size() << "  \"view\"sv.size()=" << view.size() << "\n";
    std::cout << "  1500ms = " << std::chrono::duration_cast<std::chrono::seconds>(timeout).count() << " s (truncated) = "
              << timeout.count() << " ms\n";

    constexpr Meters run = 2.5_km;       // long double verzija
    constexpr Meters lap = 400.0_m;
    constexpr Meters walk = 3_km;        // unsigned long long verzija
    std::cout << "  2.5_km=" << static_cast<double>(run.value) << " m, 400.0_m=" << static_cast<double>(lap.value)
              << " m, 3_km=" << static_cast<double>(walk.value) << " m (all at compile time, constexpr)\n";
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
