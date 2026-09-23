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
// ./check_cases.sh week3-advanced/s21-cpp17-templates  proverava ih.

// ---------------------------------------------------------------- 1
template <typename T>
struct Merenje {
    std::string senzor;
    T vrednost;
    Merenje(std::string s, T v) : senzor(std::move(s)), vrednost(v) {}
};

template <typename T>
struct Opseg {
    std::vector<T> elementi;
    template <typename It>
    Opseg(It prvi, It poslednji) : elementi(prvi, poslednji) {}
};
// Iz konstruktora se T ne vidi (It nije T) -- deduction guide kaže kako:
template <typename It>
Opseg(It, It) -> Opseg<typename std::iterator_traits<It>::value_type>;

template <typename T>
struct Par {                     // agregat: nema konstruktor
    T prvi, drugi;
};
template <typename T>
Par(T, T) -> Par<T>;             // C++17 agregat bez ovoga: errors/e02 (C++20 ne treba)

void sekcija1() {
    std::cout << "\n== 1. CTAD: argumenti šablona klase iz konstruktora\n";
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
    std::cout << "pair, tuple, vector, array, optional, lock_guard, function: tipovi provereni\n";

    Merenje m1{"temp", 21.5};                  // Merenje<double>, iz konstruktora
    static_assert(std::is_same_v<decltype(m1), Merenje<double>>);
    std::cout << "Merenje{\"temp\", 21.5}: " << m1.senzor << ' ' << m1.vrednost << '\n';

    std::vector<int> izvor{4, 5, 6};
    Opseg r(izvor.begin(), izvor.end());       // Opseg<int>, preko deduction guide-a
    static_assert(std::is_same_v<decltype(r), Opseg<int>>);
    Par par{3, 4};
    static_assert(std::is_same_v<decltype(par), Par<int>>);
    std::cout << "Opseg iz iteratora: " << r.elementi.size() << " elementa; Par{3, 4}: " << par.prvi + par.drugi
              << '\n';

    // ⚠️ Zamke:
    std::vector v2{v};                         // KOPIJA: vector<int>, ne vector<vector<int>>
    std::vector v3{v, v};                      // dva elementa: vector<vector<int>>
    static_assert(std::is_same_v<decltype(v2), std::vector<int>>);
    static_assert(std::is_same_v<decltype(v3), std::vector<std::vector<int>>>);
    std::cout << "vector{v}: size " << v2.size() << "; vector{v, v}: size " << v3.size() << '\n';

    std::pair s{"temp", 1};                    // pair<const char*, int> -- NIJE string (zadatak z3)
    static_assert(std::is_same_v<decltype(s), std::pair<const char*, int>>);
    using namespace std::string_literals;
    std::pair s2{"temp"s, 1};                  // pair<std::string, int>
    static_assert(std::is_same_v<decltype(s2), std::pair<std::string, int>>);
    std::cout << "pair{\"temp\", 1}: const char*; pair{\"temp\"s, 1}: std::string\n";
}

// ---------------------------------------------------------------- 2
template <typename... T>
int desnoUnarni(T... a) { return (a - ...); }          // a1 - (a2 - a3)
template <typename... T>
int levoUnarni(T... a) { return (... - a); }           // (a1 - a2) - a3
template <typename... T>
int desnoBinarni(T... a) { return (a - ... - 100); }   // a1 - (a2 - (a3 - 100))
template <typename... T>
int levoBinarni(T... a) { return (100 - ... - a); }    // ((100 - a1) - a2) - a3

template <typename... T>
bool svi(T... a) { return (... && a); }
template <typename... T>
bool bar_jedan(T... a) { return (... || a); }
template <typename... T>
int zbir(T... a) { return (0 + ... + a); }             // binarni: radi i za prazan paket

void sekcija2() {
    std::cout << "\n== 2. fold izrazi: četiri oblika\n";
    std::cout << "(a - ...)       za 10, 3, 2: " << desnoUnarni(10, 3, 2) << '\n';
    std::cout << "(... - a)       za 10, 3, 2: " << levoUnarni(10, 3, 2) << '\n';
    std::cout << "(a - ... - 100) za 10, 3, 2: " << desnoBinarni(10, 3, 2) << '\n';
    std::cout << "(100 - ... - a) za 10, 3, 2: " << levoBinarni(10, 3, 2) << '\n';
    std::cout << "za + redosled ne menja rezultat: " << zbir(10, 3, 2) << '\n';

    std::cout << "prazan paket: && -> " << svi() << ", || -> " << bar_jedan() << ", (0 + ... + a) -> " << zbir()
              << '\n';
    std::cout << "svi(true, 1, 2 > 1) = " << svi(true, 1, 2 > 1) << ", bar_jedan(false, 0) = " << bar_jedan(false, 0)
              << '\n';
}

// ---------------------------------------------------------------- 3
template <typename... T>
void ispisi(const T&... a) {
    const char* sep = "";
    ((std::cout << sep << a, sep = ", "), ...);        // zarez: redom, s leva na desno
    std::cout << '\n';
}

template <typename... T>
void spoji(const T&... a) {
    (std::cout << ... << a) << '\n';                   // binarni levi: ((cout << a1) << a2) ...
}

template <typename C, typename... T>
void dodajSve(C& c, T&&... a) {
    (c.push_back(std::forward<T>(a)), ...);
}

template <typename... T>
int brojPozitivnih(T... a) { return ((a > 0) + ... + 0); }

template <typename... T>
constexpr bool sviCeli = (std::is_integral_v<T> && ...);   // fold nad TIPOVIMA

void sekcija3() {
    std::cout << "\n== 3. fold u praksi: zarez, <<, poziv po elementu\n";
    ispisi(1, "dva", 3.5, 'c');
    std::cout << "binarni << fold: ";
    spoji(21, "C", '/', 3.5);
    std::vector<std::string> imena;
    dodajSve(imena, "temp", std::string("vlaga"), "pritisak");
    std::cout << "dodajSve: " << imena.size() << " elementa, poslednji " << imena.back() << '\n';
    std::cout << "brojPozitivnih(3, -1, 4, 0, 5) = " << brojPozitivnih(3, -1, 4, 0, 5) << '\n';
    static_assert(sviCeli<int, char, long>);
    static_assert(!sviCeli<int, double>);
    std::cout << "sviCeli<int, char, long> = " << sviCeli<int, char, long> << '\n';
}

// ---------------------------------------------------------------- 4
// Kako su _v i _t napravljeni (isto kao u <type_traits>):
template <typename T>
struct JeString : std::false_type {};
template <>
struct JeString<std::string> : std::true_type {};
template <typename T>
inline constexpr bool jeString_v = JeString<T>::value;       // C++17: variable template

template <typename T>
struct BezPokazivaca {
    using type = T;
};
template <typename T>
struct BezPokazivaca<T*> {
    using type = typename BezPokazivaca<T>::type;             // rekurzivno: int** -> int
};
template <typename T>
using bezPokazivaca_t = typename BezPokazivaca<T>::type;      // C++14: alias template

template <typename T>
void tipovi(T&&) {
    // Duži oblik: ::value, i typename ispred ::type (errors/e06).
    using Duze = typename std::remove_reference<T>::type;
    using Krace = std::remove_reference_t<T>;
    static_assert(std::is_same<Duze, Krace>::value == std::is_same_v<Duze, Krace>);
}

void sekcija4() {
    std::cout << "\n== 4. type traits: sufiksi _v i _t\n";
    static_assert(std::is_integral<int>::value == std::is_integral_v<int>);
    static_assert(std::is_same_v<std::remove_const_t<const int>, int>);
    int x = 0;
    tipovi(x);
    tipovi(5);
    static_assert(jeString_v<std::string> && !jeString_v<const char*>);
    static_assert(std::is_same_v<bezPokazivaca_t<int**>, int>);
    std::cout << "is_integral_v<int> = " << std::is_integral_v<int> << ", jeString_v<std::string> = "
              << jeString_v<std::string> << ", bezPokazivaca_t<int**> je int\n";
}

// ---------------------------------------------------------------- 5
template <typename T>
std::string uTekst(const T& x) {
    if constexpr (std::is_same_v<T, bool>)
        return x ? "da" : "ne";                         // mora pre is_integral: bool je integral
    else if constexpr (std::is_integral_v<T>)
        return "ceo " + std::to_string(x);
    else if constexpr (std::is_floating_point_v<T>)
        return "realan " + std::to_string(static_cast<int>(x * 10)) + "/10";
    else if constexpr (std::is_convertible_v<T, std::string>)
        return "tekst \"" + std::string(x) + '"';
    else                                                // sve ostalo: kontejner
        return "kontejner od " + std::to_string(x.size());
}

template <typename T>
auto udvostruci(const T& x) {                           // grane vraćaju RAZLIČITE tipove
    if constexpr (std::is_arithmetic_v<T>)
        return x * 2;
    else
        return x + x;
}

void sekcija5() {
    std::cout << "\n== 5. if constexpr: grana po tipu\n";
    std::cout << uTekst(true) << " | " << uTekst(42) << " | " << uTekst(2.5) << " | " << uTekst("abc") << " | "
              << uTekst(std::string("xy")) << " | " << uTekst(std::vector<int>{1, 2, 3}) << '\n';
    // Za int bi x.size() bilo greška -- ali ta grana se za int ne instancira.
    static_assert(std::is_same_v<decltype(udvostruci(3)), int>);
    static_assert(std::is_same_v<decltype(udvostruci(std::string("ab"))), std::string>);
    std::cout << "udvostruci(3) = " << udvostruci(3) << ", udvostruci(\"ab\"s) = " << udvostruci(std::string("ab"))
              << '\n';
}

// ---------------------------------------------------------------- 6
// Kraj rekurzije bez posebnog overload-a (uporedi s13, sekcija 2).
template <typename Prvi, typename... Ostali>
void ispisiRekurzivno(const Prvi& p, const Ostali&... ostali) {
    std::cout << p;
    if constexpr (sizeof...(ostali) > 0) {
        std::cout << " -> ";
        ispisiRekurzivno(ostali...);                    // za 0 ostalih se ne instancira
    } else {
        std::cout << '\n';
    }
}

template <int N>
constexpr int fakt() {
    if constexpr (N <= 1)
        return 1;
    else
        return N * fakt<N - 1>();                       // bez if constexpr: beskonačna instancijacija
}

void sekcija6() {
    std::cout << "\n== 6. if constexpr: rekurzija i kompajl-vreme\n";
    ispisiRekurzivno("temp", 21, 'C', 3.5);
    static_assert(fakt<5>() == 120);
    std::cout << "fakt<5>() = " << fakt<5>() << '\n';
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
    sekcija6();
}
