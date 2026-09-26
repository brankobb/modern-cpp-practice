#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// constexpr u C++20: consteval, constinit, std::is_constant_evaluated,
// std::vector i std::string u constexpr funkcijama. Samo C++20:
//   ./build.sh 1-language-basics/12-constexpr/main_cpp20.cpp -std=c++20

// ---------------------------------------------------------------- 1
consteval int cube(int x) { return x * x * x; } // SAMO pri kompajliranju

void s01_consteval() {
    std::cout << "-- 1. consteval: funkcija koja postoji samo pri kompajliranju --\n";
    constexpr int c = cube(3);
    int alsoCompileTime = cube(4); // i ovde: argument je konstanta, pa se izračuna pri kompajliranju
    std::cout << "  cube(3)=" << c << " cube(4)=" << alsoCompileTime
              << "  (cube(argc) se ne kompajlira, errors/e07)\n";
}

// ---------------------------------------------------------------- 2
constexpr int configuredLimit() { return 64; }
constinit int limit = configuredLimit(); // inicijalizacija pri kompajliranju, ali promenljiva NIJE const

void s02_constinit() {
    std::cout << "-- 2. constinit: statička inicijalizacija bez const --\n";
    limit += 1; // sme da se menja
    std::cout << "  constinit int limit = configuredLimit(); limit += 1 -> " << limit
              << "  (bez problema redosleda inicijalizacije, lekcija 08; dinamička: errors/e08)\n";
}

// ---------------------------------------------------------------- 3
constexpr int where() { return std::is_constant_evaluated() ? 1 : 0; }

void s03_isConstantEvaluated() {
    std::cout << "-- 3. std::is_constant_evaluated --\n";
    constexpr int atCompile = where();
    int atRuntime = where();
    std::cout << "  u constexpr promenljivoj: " << atCompile << ", u običnoj: " << atRuntime
              << "  <- ista funkcija može da bira brži algoritam za runtime\n";
}

// ---------------------------------------------------------------- 4
constexpr int sumTo(int n) {
    std::vector<int> values; // C++20: alokacija u constexpr, ako se oslobodi pre kraja izračunavanja
    for (int i = 1; i <= n; ++i) values.push_back(i);
    int sum = 0;
    for (int v : values) sum += v;
    return sum;
}

constexpr std::size_t shoutLength(const char* text) {
    std::string s = text;
    s += "!!!";
    return s.size();
}

void s04_containersInConstexpr() {
    std::cout << "-- 4. std::vector i std::string u constexpr funkciji (C++20) --\n";
    static_assert(sumTo(100) == 5050);
    static_assert(shoutLength("ovo je dugacak tekst, preko SSO granice") == 42);
    std::cout << "  sumTo(100)=5050 i shoutLength(...)=42 provereni static_assert-om\n";
    std::cout << "  <- ali constexpr std::string PROMENLJIVA sa dugačkim tekstom ne prolazi: memorija ne sme da \"preživi\" kompajliranje\n";
}

int main() {
    s01_consteval();
    s02_constinit();
    s03_isConstantEvaluated();
    s04_containersInConstexpr();
}
