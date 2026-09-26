#include <array>
#include <climits>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

// constexpr: izračunavanje pri kompajliranju -- ISPRAVNI slučajevi (C++17).
// Sve se kompajlira i radi bez ASan/UBSan prijava (g++ 13 i clang 18,
// C++17 i C++20). C++20 dodaci (consteval, constinit, std::vector i
// std::string u constexpr) su u main_cpp20.cpp:
//   ./build.sh 1-language-basics/12-constexpr/main_cpp20.cpp -std=c++20
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/12-constexpr  proverava oba.

// ---------------------------------------------------------------- 1
constexpr int square(int n) { return n * n; }

int readSensor() { return 7; } // nije constexpr: vrednost se zna tek pri izvršavanju

void s01_constexprFunctions() {
    std::cout << "-- 1. constexpr funkcija: pri kompajliranju ILI pri izvršavanju --\n";
    constexpr int atCompile = square(12);   // constexpr promenljiva MORA da se izračuna pri kompajliranju
    static_assert(atCompile == 144, "provereno pri kompajliranju");
    std::array<int, square(3)> grid{};      // i veličina niza je konstantni izraz
    int atRuntime = square(readSensor());   // ista funkcija, argument iz runtime-a -> izvršava se normalno
    std::cout << "  square(12)=" << atCompile << " (static_assert prošao), std::array<int, square(3)>.size()=" << grid.size()
              << ", square(readSensor())=" << atRuntime << "\n";
    std::cout << "  <- constexpr znači \"SME pri kompajliranju\", ne \"MORA\"; mora samo kad rezultat traži konstantu\n";
}

// ---------------------------------------------------------------- 2
// C++14: constexpr funkcije smeju da imaju petlje, lokalne promenljive, if.
constexpr std::uint64_t factorial(int n) {
    std::uint64_t result = 1;
    for (int i = 2; i <= n; ++i) result *= static_cast<std::uint64_t>(i);
    return result;
}

constexpr bool isPrime(int n) {
    if (n < 2) return false;
    for (int d = 2; d * d <= n; ++d)
        if (n % d == 0) return false;
    return true;
}

void s02_loopsAndLocals() {
    std::cout << "-- 2. petlje i lokalne promenljive (C++14) --\n";
    constexpr std::uint64_t f20 = factorial(20);
    static_assert(isPrime(97) && !isPrime(91), "provera prostih brojeva pri kompajliranju");
    std::cout << "  factorial(20)=" << f20 << ", isPrime(97) i !isPrime(91) provereni static_assert-om\n";
}

// ---------------------------------------------------------------- 3
constexpr int add(int a, int b) { return a + b; }

void s03_ubIsAnError() {
    std::cout << "-- 3. UB u konstantnom izrazu je GREŠKA pri kompajliranju --\n";
    constexpr int ok = add(INT_MAX - 1, 1);
    // constexpr int bad = add(INT_MAX, 1);  // ne kompajlira se: overflow (errors/e01)
    std::cout << "  add(INT_MAX - 1, 1)=" << ok << "; add(INT_MAX, 1) u constexpr -> greška (errors/e01),\n"
              << "  a pri izvršavanju -> tihi UB, samo UBSan ga vidi (ub/u01)\n";
}

// ---------------------------------------------------------------- 4
struct Point { // literal tip: constexpr konstruktor, constexpr funkcije članice
    int x;
    int y;
    constexpr Point(int px, int py) : x(px), y(py) {}
    constexpr Point operator+(Point other) const { return Point(x + other.x, y + other.y); }
    constexpr int manhattan() const { return (x < 0 ? -x : x) + (y < 0 ? -y : y); }
};

constexpr Point origin(0, 0);
constexpr Point target = origin + Point(3, -4);
static_assert(target.manhattan() == 7, "Point radi pri kompajliranju");

constexpr std::string_view unitName = "kilometar"; // string_view je literal tip (C++17), std::string nije (errors/e06)

void s04_literalTypes() {
    std::cout << "-- 4. literal tipovi: sopstvene klase u constexpr --\n";
    std::cout << "  constexpr Point target = origin + Point(3, -4) -> (" << target.x << ", " << target.y
              << "), manhattan=" << target.manhattan() << "; constexpr string_view: \"" << unitName << "\" (" << unitName.size()
              << ")\n";
}

// ---------------------------------------------------------------- 5
template <std::size_t N>
constexpr std::array<std::uint32_t, N> makeSquares() {
    std::array<std::uint32_t, N> table{};
    for (std::size_t i = 0; i < N; ++i) table[i] = static_cast<std::uint32_t>(i * i);
    return table;
}

constexpr auto squares = makeSquares<16>(); // tabela u read-only memoriji; na mikrokontroleru ide u flash

void s05_lookupTables() {
    std::cout << "-- 5. tabele izračunate pri kompajliranju --\n";
    static_assert(squares[15] == 225, "tabela je gotova pre pokretanja programa");
    std::cout << "  squares[7]=" << squares[7] << " squares[15]=" << squares[15] << " (nijedna instrukcija za računanje pri pokretanju)\n";
}

// ---------------------------------------------------------------- 6
template <typename T>
std::string describe(const T& value) {
    if constexpr (std::is_same_v<T, std::string>) {
        return "string dužine " + std::to_string(value.size()); // za T = int se ova grana ni ne kompajlira
    } else if constexpr (std::is_integral_v<T>) {
        return "ceo broj " + std::to_string(value);
    } else {
        return "nešto drugo";
    }
}

void s06_ifConstexpr() {
    std::cout << "-- 6. if constexpr (C++17) --\n";
    std::cout << "  describe(string)=\"" << describe(std::string("abc")) << "\" describe(42)=\"" << describe(42)
              << "\" describe(1.5)=\"" << describe(1.5) << "\"\n";
    std::cout << "  <- običan if bi kompajlirao OBE grane za svaki T, pa value.size() za int ne prolazi (errors/e05)\n";
}

// ---------------------------------------------------------------- 7
template <typename T>
struct Register {
    static_assert(std::is_trivially_copyable_v<T>, "Register: tip mora biti trivijalno kopirljiv");
    static_assert(sizeof(T) == 4, "Register: tip mora imati tačno 4 bajta"); // (errors/e09)
    T value;
};

void s07_staticAssert() {
    std::cout << "-- 7. static_assert: pravila proverena pri kompajliranju --\n";
    Register<std::uint32_t> reg{0xABCD};
    std::cout << "  Register<std::uint32_t> prošao obe provere, value=0x" << std::hex << reg.value << std::dec
              << "; Register<std::uint16_t> se ne kompajlira (errors/e09)\n";
}

int main() {
    s01_constexprFunctions();
    s02_loopsAndLocals();
    s03_ubIsAnError();
    s04_literalTypes();
    s05_lookupTables();
    s06_ifConstexpr();
    s07_staticAssert();
}
