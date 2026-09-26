#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <mutex>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// C++17 novine za šablone: CTAD, fold izrazi, _v/_t traits, if constexpr
// -- ISPRAVNI slučajevi. Sve se kompajlira bez upozorenja i radi bez
// ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20). Brojevi sekcija
// prate notes.md. Tipovi se proveravaju sa static_assert -- ako se
// kompajlira, dedukcija je dala baš taj tip.
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
// ./check_cases.sh 4-templates/29-cpp17-templates  proverava ih.

// ---------------------------------------------------------------- 1
template <typename T>
struct Measurement {
    std::string sensor;
    T value;
    Measurement(std::string s, T v) : sensor(std::move(s)), value(v) {}
};

template <typename T>
struct Range {
    std::vector<T> items;
    template <typename It>
    Range(It first, It last) : items(first, last) {}
};
// Iz konstruktora se T ne vidi (It nije T) -- deduction guide kaže kako:
template <typename It>
Range(It, It) -> Range<typename std::iterator_traits<It>::value_type>;

template <typename T>
struct Pair {                     // agregat: nema konstruktor
    T first, second;
};
template <typename T>
Pair(T, T) -> Pair<T>;             // C++17 agregat bez ovoga: errors/e02 (C++20 ne treba)

void section1() {
    std::cout << "\n== 1. CTAD: class template arguments from the constructor\n";
    std::pair p{1, 2.5};                       // pair<int, double>
    std::tuple t{1, 'x', 3.0};                 // tuple<int, char, double>
    std::vector v{1, 2, 3};                    // vector<int>
    std::array a{1, 2, 3};                     // array<int, 3>
    std::optional o{5};                        // optional<int>
    std::mutex m;
    std::lock_guard g(m);                      // lock_guard<std::mutex>
    std::function f = [](int x) { return x * 2.0; };   // function<double(int)>
    static_assert(std::is_same_v<decltype(p), std::pair<int, double>>);
    static_assert(std::is_same_v<decltype(t), std::tuple<int, char, double>>);
    static_assert(std::is_same_v<decltype(v), std::vector<int>>);
    static_assert(std::is_same_v<decltype(a), std::array<int, 3>>);
    static_assert(std::is_same_v<decltype(o), std::optional<int>>);
    static_assert(std::is_same_v<decltype(f), std::function<double(int)>>);
    std::cout << "pair, tuple, vector, array, optional, lock_guard, function: types checked\n";

    Measurement m1{"temp", 21.5};                  // Measurement<double>, iz konstruktora
    static_assert(std::is_same_v<decltype(m1), Measurement<double>>);
    std::cout << "Measurement{\"temp\", 21.5}: " << m1.sensor << ' ' << m1.value << '\n';

    std::vector<int> source{4, 5, 6};
    Range r(source.begin(), source.end());       // Range<int>, preko deduction guide-a
    static_assert(std::is_same_v<decltype(r), Range<int>>);
    Pair pr{3, 4};
    static_assert(std::is_same_v<decltype(pr), Pair<int>>);
    std::cout << "Range from iterators: " << r.items.size() << " elements; Pair{3, 4}: " << pr.first + pr.second
              << '\n';

    // ⚠️ Zamke:
    std::vector v2{v};                         // KOPIJA: vector<int>, ne vector<vector<int>>
    std::vector v3{v, v};                      // dva elementa: vector<vector<int>>
    static_assert(std::is_same_v<decltype(v2), std::vector<int>>);
    static_assert(std::is_same_v<decltype(v3), std::vector<std::vector<int>>>);
    std::cout << "vector{v}: size " << v2.size() << "; vector{v, v}: size " << v3.size() << '\n';

    std::pair s{"temp", 1};                    // pair<const char*, int> -- NIJE string (zadatak ex3)
    static_assert(std::is_same_v<decltype(s), std::pair<const char*, int>>);
    using namespace std::string_literals;
    std::pair s2{"temp"s, 1};                  // pair<std::string, int>
    static_assert(std::is_same_v<decltype(s2), std::pair<std::string, int>>);
    std::cout << "pair{\"temp\", 1}: const char*; pair{\"temp\"s, 1}: std::string\n";
}

// ---------------------------------------------------------------- 2
template <typename... T>
int rightUnary(T... a) { return (a - ...); }          // a1 - (a2 - a3)
template <typename... T>
int leftUnary(T... a) { return (... - a); }           // (a1 - a2) - a3
template <typename... T>
int rightBinary(T... a) { return (a - ... - 100); }   // a1 - (a2 - (a3 - 100))
template <typename... T>
int leftBinary(T... a) { return (100 - ... - a); }    // ((100 - a1) - a2) - a3

template <typename... T>
bool allTrue(T... a) { return (... && a); }
template <typename... T>
bool anyTrue(T... a) { return (... || a); }
template <typename... T>
int sum(T... a) { return (0 + ... + a); }             // binarni: radi i za prazan paket

void section2() {
    std::cout << "\n== 2. fold expressions: four forms\n";
    std::cout << "(a - ...)       for 10, 3, 2: " << rightUnary(10, 3, 2) << '\n';
    std::cout << "(... - a)       for 10, 3, 2: " << leftUnary(10, 3, 2) << '\n';
    std::cout << "(a - ... - 100) for 10, 3, 2: " << rightBinary(10, 3, 2) << '\n';
    std::cout << "(100 - ... - a) for 10, 3, 2: " << leftBinary(10, 3, 2) << '\n';
    std::cout << "for + the order does not change the result: " << sum(10, 3, 2) << '\n';

    std::cout << "empty pack: && -> " << allTrue() << ", || -> " << anyTrue() << ", (0 + ... + a) -> " << sum()
              << '\n';
    std::cout << "allTrue(true, 1, 2 > 1) = " << allTrue(true, 1, 2 > 1) << ", anyTrue(false, 0) = " << anyTrue(false, 0)
              << '\n';
}

// ---------------------------------------------------------------- 3
template <typename... T>
void print(const T&... a) {
    const char* sep = "";
    ((std::cout << sep << a, sep = ", "), ...);        // zarez: redom, s leva na desno
    std::cout << '\n';
}

template <typename... T>
void concat(const T&... a) {
    (std::cout << ... << a) << '\n';                   // binarni levi: ((cout << a1) << a2) ...
}

template <typename C, typename... T>
void pushAll(C& c, T&&... a) {
    (c.push_back(std::forward<T>(a)), ...);
}

template <typename... T>
int countPositive(T... a) { return ((a > 0) + ... + 0); }

template <typename... T>
constexpr bool allIntegral = (std::is_integral_v<T> && ...);   // fold nad TIPOVIMA

void section3() {
    std::cout << "\n== 3. fold in practice: comma, <<, a call per element\n";
    print(1, "two", 3.5, 'c');
    std::cout << "binary << fold: ";
    concat(21, "C", '/', 3.5);
    std::vector<std::string> names;
    pushAll(names, "temp", std::string("humidity"), "pressure");
    std::cout << "pushAll: " << names.size() << " elements, last " << names.back() << '\n';
    std::cout << "countPositive(3, -1, 4, 0, 5) = " << countPositive(3, -1, 4, 0, 5) << '\n';
    static_assert(allIntegral<int, char, long>);
    static_assert(!allIntegral<int, double>);
    std::cout << "allIntegral<int, char, long> = " << allIntegral<int, char, long> << '\n';
}

// ---------------------------------------------------------------- 4
// Kako su _v i _t napravljeni (isto kao u <type_traits>):
template <typename T>
struct IsString : std::false_type {};
template <>
struct IsString<std::string> : std::true_type {};
template <typename T>
inline constexpr bool isString_v = IsString<T>::value;       // C++17: variable template

template <typename T>
struct RemoveAllPointers {
    using type = T;
};
template <typename T>
struct RemoveAllPointers<T*> {
    using type = typename RemoveAllPointers<T>::type;             // rekurzivno: int** -> int
};
template <typename T>
using removeAllPointers_t = typename RemoveAllPointers<T>::type;      // C++14: alias template

template <typename T>
void checkTypes(T&&) {
    // Duži oblik: ::value, i typename ispred ::type (errors/e06).
    using Longer = typename std::remove_reference<T>::type;
    using Shorter = std::remove_reference_t<T>;
    static_assert(std::is_same<Longer, Shorter>::value == std::is_same_v<Longer, Shorter>);
}

void section4() {
    std::cout << "\n== 4. type traits: suffixes _v and _t\n";
    static_assert(std::is_integral<int>::value == std::is_integral_v<int>);
    static_assert(std::is_same_v<std::remove_const_t<const int>, int>);
    int x = 0;
    checkTypes(x);
    checkTypes(5);
    static_assert(isString_v<std::string> && !isString_v<const char*>);
    static_assert(std::is_same_v<removeAllPointers_t<int**>, int>);
    std::cout << "is_integral_v<int> = " << std::is_integral_v<int> << ", isString_v<std::string> = "
              << isString_v<std::string> << ", removeAllPointers_t<int**> is int\n";
}

// ---------------------------------------------------------------- 5
template <typename T>
std::string toText(const T& x) {
    if constexpr (std::is_same_v<T, bool>)
        return x ? "yes" : "no";                         // mora pre is_integral: bool je integral
    else if constexpr (std::is_integral_v<T>)
        return "integer " + std::to_string(x);
    else if constexpr (std::is_floating_point_v<T>)
        return "real " + std::to_string(static_cast<int>(x * 10)) + "/10";
    else if constexpr (std::is_convertible_v<T, std::string>)
        return "text \"" + std::string(x) + '"';
    else                                                // sve ostalo: kontejner
        return "container of " + std::to_string(x.size());
}

template <typename T>
auto doubled(const T& x) {                           // grane vraćaju RAZLIČITE tipove
    if constexpr (std::is_arithmetic_v<T>)
        return x * 2;
    else
        return x + x;
}

void section5() {
    std::cout << "\n== 5. if constexpr: a branch per type\n";
    std::cout << toText(true) << " | " << toText(42) << " | " << toText(2.5) << " | " << toText("abc") << " | "
              << toText(std::string("xy")) << " | " << toText(std::vector<int>{1, 2, 3}) << '\n';
    // Za int bi x.size() bilo greška -- ali ta grana se za int ne instancira.
    static_assert(std::is_same_v<decltype(doubled(3)), int>);
    static_assert(std::is_same_v<decltype(doubled(std::string("ab"))), std::string>);
    std::cout << "doubled(3) = " << doubled(3) << ", doubled(\"ab\"s) = " << doubled(std::string("ab"))
              << '\n';
}

// ---------------------------------------------------------------- 6
// Kraj rekurzije bez posebnog overload-a (uporedi lekcija 28, sekcija 2).
template <typename First, typename... Rest>
void printRecursive(const First& p, const Rest&... rest) {
    std::cout << p;
    if constexpr (sizeof...(rest) > 0) {
        std::cout << " -> ";
        printRecursive(rest...);                    // za 0 ostalih se ne instancira
    } else {
        std::cout << '\n';
    }
}

template <int N>
constexpr int fact() {
    if constexpr (N <= 1)
        return 1;
    else
        return N * fact<N - 1>();                       // bez if constexpr: beskonačna instancijacija
}

void section6() {
    std::cout << "\n== 6. if constexpr: recursion and compile time\n";
    printRecursive("temp", 21, 'C', 3.5);
    static_assert(fact<5>() == 120);
    std::cout << "fact<5>() = " << fact<5>() << '\n';
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
}
