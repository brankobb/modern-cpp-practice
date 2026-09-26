#include <cctype>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

// auto i dedukcija tipova -- ISPRAVNI slučajevi. Sve se kompajlira i radi
// bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/10-auto-and-range-for  proverava oba.
// Warning u sekciji 1 (petlja preko mape sa pogrešnim tipom) je namerni.

// EMC Item 4: prikaz dedukovanog tipa. __PRETTY_FUNCTION__ (g++ i clang;
// MSVC ima __FUNCSIG__) sadrži ime funkcije sa konkretnim T. g++ i clang ga
// formatiraju malo drugačije, pa se razmaci ovde svode na isti oblik.
template <typename T>
std::string typeName() {
    std::string s = __PRETTY_FUNCTION__;
    std::size_t start = s.find("T = ") + 4;
    std::size_t end = s.find(';', start);
    if (end == std::string::npos) end = s.rfind(']');
    std::string raw = s.substr(start, end - start), t;
    const std::string punct = "&*[]()";
    for (std::size_t i = 0; i < raw.size(); ++i) {
        bool nearPunct = (i > 0 && punct.find(raw[i - 1]) != std::string::npos) ||
                         (i + 1 < raw.size() && punct.find(raw[i + 1]) != std::string::npos);
        if (raw[i] == ' ' && nearPunct) continue;
        if (!t.empty() && (t.back() == '*' || t.back() == '&') &&
            std::isalpha(static_cast<unsigned char>(raw[i]))) {
            t += ' ';
        }
        t += raw[i];
    }
    return t;
}

// ---------------------------------------------------------------- 1
void s01_whyAuto() {
    std::cout << "-- 1. why auto (EMC Item 5) --\n";
    std::map<std::string, int> ages{{"Ann", 30}};
    const std::string* stored = &ages.begin()->first;

    // Element mape je std::pair<const std::string, int>, a ne
    // std::pair<std::string, int>. Sa pogrešno napisanim tipom kompajler
    // za SVAKI element pravi privremenu kopiju, a referenca se veže za nju.
    for (const std::pair<std::string, int>& p : ages) {
        std::cout << "wrong type:   &p.first == address in the map? " << std::boolalpha
                  << (&p.first == stored) << "  (a copy!)\n";
    }
    for (const auto& p : ages) {
        std::cout << "const auto&:  &p.first == address in the map? " << (&p.first == stored)
                  << std::noboolalpha << "\n";
    }

    // Tip lambde zna samo kompajler -- bez auto ne možeš ni da ga napišeš.
    // std::function može da ga čuva, ali je veći i poziva se indirektno.
    auto square = [](int x) { return x * x; };
    std::function<int(int)> boxed = square;
    std::cout << "sizeof(lambda)=" << sizeof(square) << " sizeof(std::function)=" << sizeof(boxed)
              << " (library dependent), result " << square(4) << "/" << boxed(4) << "\n";
}

// ---------------------------------------------------------------- 2
template <typename T>
void byRef(T& param) {
    std::cout << "    T=" << typeName<T>() << "   param=" << typeName<decltype(param)>() << "\n";
}
template <typename T>
void byUniversal(T&& param) {
    std::cout << "    T=" << typeName<T>() << "   param=" << typeName<decltype(param)>() << "\n";
}
template <typename T>
void byValue(T param) {
    std::cout << "    T=" << typeName<T>() << "   param=" << typeName<decltype(param)>() << "\n";
}

void someFunc(int) {}

template <typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept { // T(&)[N]: referenca čuva veličinu niza
    return N;
}

void s02_templateDeduction() {
    std::cout << "-- 2. template type deduction (EMC Item 1) --\n";
    int x = 27;
    const int cx = x;
    const int& rx = x;
    const char* const ptr = "text";
    const char name[] = "J. P. Briggs";

    std::cout << "  case 1 -- f(T& param): the reference is dropped, const stays\n";
    byRef(x);
    byRef(cx);
    byRef(rx);

    std::cout << "  case 2 -- f(T&& param) (universal reference): lvalue -> T is a reference\n";
    byUniversal(x);
    byUniversal(cx);
    byUniversal(rx);
    byUniversal(27);

    std::cout << "  case 3 -- f(T param) (by value): reference and const are dropped (the copy is new)\n";
    byValue(x);
    byValue(cx);
    byValue(rx);
    byValue(ptr); // const na POKAZIVAČU (top-level) nestaje, const na podacima ostaje

    std::cout << "  arrays and functions: by value they decay to a pointer, by reference they do not\n";
    byValue(name);
    byRef(name);
    byValue(someFunc);
    byRef(someFunc);
    static_assert(arraySize(name) == 13);
    std::cout << "  arraySize(name) = " << arraySize(name) << " (computed at compile time)\n";
}

// ---------------------------------------------------------------- 3
void s03_autoDeduction() {
    std::cout << "-- 3. auto type deduction (EMC Item 2) --\n";
    int x = 27;
    const int cx = x;
    const char name[] = "J. P. Briggs";

    auto a = x;           // kao f(T): int
    const auto b = x;     // const int
    const auto& c = x;    // const int&
    auto&& d = x;         // kao f(T&&) sa lvalue: int&
    auto&& e = cx;        // const int&
    auto&& f = 27;        // rvalue: int&&
    auto g = name;        // niz se raspada: const char*
    auto& h = name;       // const char(&)[13]
    auto i = someFunc;    // void(*)(int)
    auto& j = someFunc;   // void(&)(int)
    const auto& k = 42;   // const& se veže i za privremeni (auto& ne -- errors/e05)
    auto l = {27};        // JEDINA razlika u odnosu na template: std::initializer_list<int>
    auto m{27};           // C++17: int (lekcija 03)

    std::cout << "  auto a = x          -> " << typeName<decltype(a)>() << "\n"
              << "  const auto b = x    -> " << typeName<decltype(b)>() << "\n"
              << "  const auto& c = x   -> " << typeName<decltype(c)>() << "\n"
              << "  auto&& d = x        -> " << typeName<decltype(d)>() << "\n"
              << "  auto&& e = cx       -> " << typeName<decltype(e)>() << "\n"
              << "  auto&& f = 27       -> " << typeName<decltype(f)>() << "\n"
              << "  auto g = name       -> " << typeName<decltype(g)>() << "\n"
              << "  auto& h = name      -> " << typeName<decltype(h)>() << "\n"
              << "  auto i = someFunc   -> " << typeName<decltype(i)>() << "\n"
              << "  auto& j = someFunc  -> " << typeName<decltype(j)>() << "\n"
              << "  const auto& k = 42  -> " << typeName<decltype(k)>() << "\n"
              << "  auto l = {27}       -> " << (std::is_same_v<decltype(l), std::initializer_list<int>>
                                                   ? "std::initializer_list<int>" : "?") << "\n"
              << "  auto m{27}          -> " << typeName<decltype(m)>() << "\n";
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f; (void)g; (void)h; (void)i; (void)j;
}

// ---------------------------------------------------------------- 4
template <typename Container, typename Index>
decltype(auto) authAndAccess(Container&& c, Index i) { // EMC Item 3
    return std::forward<Container>(c)[i];               // vraća TAČNO ono što vraća operator[]
}

void s04_decltype() {
    std::cout << "-- 4. decltype i decltype(auto) (EMC Item 3) --\n";
    int x = 0;
    const int& cw = x;
    std::cout << "  decltype(x)   -> " << typeName<decltype(x)>() << "\n"
              << "  decltype((x)) -> " << typeName<decltype((x))>() << "   <- (x) is an expression (lvalue), not a name\n"
              << "  decltype(cw)  -> " << typeName<decltype(cw)>() << "\n";

    auto copy = cw;           // auto: const i & se odbacuju
    decltype(auto) same = cw; // decltype(auto): tip TAČNO kao izraz
    std::cout << "  auto copy = cw           -> " << typeName<decltype(copy)>() << "\n"
              << "  decltype(auto) same = cw -> " << typeName<decltype(same)>() << "\n";

    std::vector<int> v{1, 2, 3};
    authAndAccess(v, 0) = 10; // vraća int& -- dodela menja v (sa auto: greška, errors/e12)
    std::cout << "  authAndAccess(v, 0) = 10 -> v[0]=" << v[0] << " (tip: "
              << typeName<decltype(authAndAccess(v, 0))>() << ")\n";
    // decltype(auto) + return (x); vraća referencu na lokalnu -- ub/u02.
}

// ---------------------------------------------------------------- 5
void s05_viewTypes() {
    std::cout << "-- 5. how to see the deduced type (EMC Item 4) --\n";
    const int x = 0;
    auto& y = x;
    std::cout << "  typeName<decltype(y)>()                = " << typeName<decltype(y)>() << "\n";
    // typeid NIJE pouzdan: po pravilima jezika odbacuje referencu i const.
    std::cout << "  typeid(const int&) == typeid(int)?       " << std::boolalpha
              << (typeid(const int&) == typeid(int)) << std::noboolalpha << "  <- typeid lies\n";
    // Najpouzdanije: namerna greška -- TD<decltype(y)> (errors/e08).
}

// ---------------------------------------------------------------- 6
void s06_whenAutoLies() {
    std::cout << "-- 6. when auto deduces the wrong type (EMC Item 6) --\n";
    std::vector<bool> flags(10, true);
    auto proxy = flags[5]; // NIJE bool, nego std::vector<bool>::reference (proxy)
    static_assert(!std::is_same_v<decltype(proxy), bool>);
    static_assert(std::is_same_v<decltype(proxy), std::vector<bool>::reference>);
    proxy = false;         // menja VEKTOR, ne kopiju!
    std::cout << "  auto proxy = flags[5]; proxy = false; -> flags[5]=" << flags[5]
              << "  (the proxy writes into the vector)\n";

    bool copy = flags[6];                          // eksplicitan tip: prava kopija
    auto alsoCopy = static_cast<bool>(flags[7]);   // "explicitly typed initializer" idiom
    copy = false;
    alsoCopy = false;
    std::cout << "  bool/static_cast<bool> copies = " << copy << "," << alsoCopy
              << " -> flags[6]=" << flags[6] << " flags[7]=" << flags[7] << " (vector untouched)\n";
    // Proxy od PRIVREMENOG vektora visi -- ub/u01.
}

// ---------------------------------------------------------------- 7
struct Item {
    int value;
    void doubleIt() { value *= 2; }
};

std::vector<int> makeVector() { return {4, 5, 6}; }

void s07_rangeFor() {
    std::cout << "-- 7. range-for --\n";
    std::vector<Item> items{{1}, {2}, {3}};

    for (auto item : items) item.doubleIt();   // kopija -- original se NE menja
    std::cout << "  after for (auto item ...):  ";
    for (const auto& item : items) std::cout << item.value << ' ';
    std::cout << "\n";

    for (auto& item : items) item.doubleIt();  // referenca -- menja original
    std::cout << "  after for (auto& item ...): ";
    for (const auto& item : items) std::cout << item.value << ' ';
    std::cout << "\n";

    std::vector<bool> flags{true, false, true};
    for (auto&& b : flags) b = !b;              // auto&& radi i sa proxy-jem (auto& ne -- errors/e06)
    std::cout << "  vector<bool> after for (auto&& b ...): ";
    for (bool b : flags) std::cout << b << ' ';
    std::cout << "\n";

    std::map<std::string, int> ages{{"Ann", 30}, {"John", 25}};
    for (const auto& [name, age] : ages) {      // C++17 structured bindings
        std::cout << "  " << name << "=" << age;
    }
    std::cout << "\n";

    int sum = 0;
    for (int v : makeVector()) sum += v;        // OK: život vraćenog vektora se produžava
    std::cout << "  for (int v : makeVector()) sum=" << sum
              << "  (but Holder{}.items() dangles -- ub/u03)\n";
}

// ---------------------------------------------------------------- 8
auto twice(int x) { return 2 * x; }               // C++14: tip povratne vrednosti iz return
auto add(int a, int b) -> int { return a + b; }   // trailing return type (C++11)

void s08_otherPlaces() {
    std::cout << "-- 8. auto in other places --\n";
    static_assert(std::is_same_v<decltype(twice(1)), int>);
    auto plus = [](auto a, auto b) { return a + b; }; // generička lambda (C++14)
    std::cout << "  twice(21)=" << twice(21) << " add(2,3)=" << add(2, 3)
              << " plus(1,2)=" << plus(1, 2) << " plus(string)=" << plus(std::string("ab"), std::string("cd")) << "\n";

    std::set<int> numbers{1, 2};
    auto [it, inserted] = numbers.insert(2);      // structured binding za std::pair
    auto [it2, inserted2] = numbers.insert(3);
    std::cout << "  insert(2): inserted=" << std::boolalpha << inserted << " (*it=" << *it
              << "), insert(3): inserted=" << inserted2 << " (*it2=" << *it2 << ")"
              << std::noboolalpha << "\n";
}

int main() {
    s01_whyAuto();
    s02_templateDeduction();
    s03_autoDeduction();
    s04_decltype();
    s05_viewTypes();
    s06_whenAutoLies();
    s07_rangeFor();
    s08_otherPlaces();
}
