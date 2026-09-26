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
    std::cout << "-- 1. consteval: a function that exists only at compile time --\n";
    constexpr int c = cube(3);
    int alsoCompileTime = cube(4); // i ovde: argument je konstanta, pa se izračuna pri kompajliranju
    std::cout << "  cube(3)=" << c << " cube(4)=" << alsoCompileTime
              << "  (cube(argc) does not compile, errors/e07)\n";
}

// ---------------------------------------------------------------- 2
constexpr int configuredLimit() { return 64; }
constinit int limit = configuredLimit(); // inicijalizacija pri kompajliranju, ali promenljiva NIJE const

void s02_constinit() {
    std::cout << "-- 2. constinit: static initialization without const --\n";
    limit += 1; // sme da se menja
    std::cout << "  constinit int limit = configuredLimit(); limit += 1 -> " << limit
              << "  (no initialization order problem, lesson 08; dynamic: errors/e08)\n";
}

// ---------------------------------------------------------------- 3
constexpr int where() { return std::is_constant_evaluated() ? 1 : 0; }

void s03_isConstantEvaluated() {
    std::cout << "-- 3. std::is_constant_evaluated --\n";
    constexpr int atCompile = where();
    int atRuntime = where();
    std::cout << "  in a constexpr variable: " << atCompile << ", in a plain one: " << atRuntime
              << "  <- the same function can pick a faster algorithm for run time\n";
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
    std::cout << "-- 4. std::vector and std::string in a constexpr function (C++20) --\n";
    static_assert(sumTo(100) == 5050);
    static_assert(shoutLength("this is a long text, past the SSO limit") == 42);
    std::cout << "  sumTo(100)=5050 and shoutLength(...)=42 checked by static_assert\n";
    std::cout << "  <- but a constexpr std::string VARIABLE with a long text fails: the memory must not \"survive\" compilation\n";
}

int main() {
    s01_consteval();
    s02_constinit();
    s03_isConstantEvaluated();
    s04_containersInConstexpr();
}
