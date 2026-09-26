#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>
#if __cplusplus >= 202002L
#include <bit>
#endif

// Složeni (compound) tipovi -- ISPRAVNI slučajevi. Sve se kompajlira i radi
// bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/05-compound-types  proverava oba.
// Pokazivači i reference su u lekciji 04, klase u lekcijama 14–17.

enum Color { Red, Green, Blue };          // obična (unscoped) enum
enum class Status { Ok, Error };          // scoped enum
union Number {
    int i;
    float f;
};
struct Point {
    int x = 0;
    int y = 0;
};

// ---------------------------------------------------------------- 1
template <typename T>
std::string category() {
    if (std::is_void_v<T>) return "void";
    if (std::is_null_pointer_v<T>) return "std::nullptr_t";
    if (std::is_integral_v<T>) return "celobrojni";
    if (std::is_floating_point_v<T>) return "realni";
    if (std::is_array_v<T>) return "niz";
    if (std::is_function_v<T>) return "funkcija";
    if (std::is_member_pointer_v<T>) return "pokazivač na člana";
    if (std::is_pointer_v<T>) return "pokazivač";
    if (std::is_lvalue_reference_v<T>) return "lvalue referenca";
    if (std::is_rvalue_reference_v<T>) return "rvalue referenca";
    if (std::is_enum_v<T>) return "enum";
    if (std::is_union_v<T>) return "union";
    if (std::is_class_v<T>) return "klasa";
    return "?";
}

template <typename T>
void classify(const char* name) {
    std::cout << "  " << name << (std::is_fundamental_v<T> ? "  -> fundamentalni, " : "  -> SLOŽENI, ")
              << category<T>() << "\n";
}

void s01_typeCategories() {
    std::cout << "-- 1. podela tipova --\n";
    classify<int>("int");
    classify<bool>("bool");
    classify<double>("double");
    classify<void>("void");
    classify<std::nullptr_t>("std::nullptr_t");
    classify<int[3]>("int[3]");
    classify<int*>("int*");
    classify<int&>("int&");
    classify<int&&>("int&&");
    classify<void(int)>("void(int)");
    classify<int Point::*>("int Point::*");
    classify<Color>("Color");
    classify<Status>("Status");
    classify<Number>("Number");
    classify<std::string>("std::string");
    static_assert(std::is_compound_v<int*> && !std::is_compound_v<int>);
}

// ---------------------------------------------------------------- 2
void s02_cArrays() {
    std::cout << "-- 2. C nizovi --\n";
    int a[5] = {1, 2, 3};      // ostatak se popuni nulama
    int b[]{4, 5, 6};          // veličina 3 se dedukuje
    char s[] = "abc";          // 4 elementa: 'a','b','c','\0'
    std::cout << "  int a[5]={1,2,3}: ";
    for (int v : a) std::cout << v << ' ';
    std::cout << "  std::size(b)=" << std::size(b) << "  sizeof(\"abc\" niz)=" << sizeof(s) << " (i '\\0')\n";

    int grid[2][3] = {{1, 2, 3}, {4, 5, 6}}; // niz od 2 niza od po 3 int-a, redom u memoriji
    std::cout << "  grid[1][2]=" << grid[1][2] << " sizeof(grid)=" << sizeof(grid) << "\n";

    // a = b; ne radi (errors/e03). Kopija i poređenje -- element po element:
    int copy[3];
    std::copy(std::begin(b), std::end(b), std::begin(copy));
    bool same = std::equal(std::begin(b), std::end(b), std::begin(copy));
    std::cout << "  std::copy + std::equal: " << std::boolalpha << same << std::noboolalpha
              << "  (a == b bi poredio ADRESE, ne sadržaj)\n";
}

// ---------------------------------------------------------------- 3
std::array<int, 3> makeArray() { return {7, 8, 9}; } // std::array SME da se vrati (errors/e04 za C niz)

void s03_stdArray() {
    std::cout << "-- 3. std::array --\n";
    std::array<int, 3> a{1, 2, 3};
    auto b = a;          // kopija
    b[0] = 99;
    std::cout << "  posle b = a; b[0] = 99: a[0]=" << a[0] << " b[0]=" << b[0]
              << "  a == b: " << std::boolalpha << (a == b) << std::noboolalpha << " (poredi sadržaj)\n";
    a = makeArray();     // dodela radi
    std::cout << "  a = makeArray(): " << a[0] << a[1] << a[2] << "  a.size()=" << a.size()
              << "  *a.data()=" << *a.data() << " (pokazivač za C API)\n";
    try {
        std::cout << a.at(5);                     // proverava granice
    } catch (const std::out_of_range&) {
        std::cout << "  a.at(5) -> std::out_of_range (a[5] bi bio UB -- ub/u01)\n";
    }
    std::array c{1, 2, 3};                        // C++17 CTAD -> std::array<int, 3>
    static_assert(std::is_same_v<decltype(c), std::array<int, 3>>);
    static_assert(sizeof(std::array<int, 3>) == sizeof(int[3])); // bez dodatne cene
}

// ---------------------------------------------------------------- 4
template <typename E>
constexpr std::underlying_type_t<E> toUType(E e) noexcept { // EMC Item 10
    return static_cast<std::underlying_type_t<E>>(e);
}

enum class Small : std::uint8_t { A, B };  // izabran tip -> 1 bajt
enum class Later;                          // enum class sme unapred da se deklariše (int)
enum class Later { First, Second };

enum class UserField { Name, Email, Reputation };

std::string describe(Status s) {
    switch (s) { // -Wall (-Wswitch) upozori ako neki element nije obrađen
        case Status::Ok: return "Ok";
        case Status::Error: return "Error";
    }
    return "?";
}

void s04_enums() {
    std::cout << "-- 4. enum i enum class (EMC Item 10) --\n";
    // Obična enum: imena "cure" u okolni scope, a vrednost se tiho pretvara u int.
    int n = Green;                     // 1 -- bez cast-a
    Color c = Blue;
    bool weird = c < 14.5;             // kompajlira se: Color -> int -> double
                                       // (C++20 ovo proglašava zastarelim -- oba kompajlera upozore)
    std::cout << "  obična: int n = Green -> " << n << "; Blue < 14.5 -> " << std::boolalpha << weird << "\n";

    // enum class: imena u svom scope-u, bez implicitne konverzije (errors/e07, e08, e09).
    Status st = Status::Error;
    std::cout << "  enum class: " << describe(st) << " = " << static_cast<int>(st)
              << "; sizeof(Small)=" << sizeof(Small) << " sizeof(Status)=" << sizeof(Status) << "\n";
    static_assert(std::is_same_v<std::underlying_type_t<Status>, int>);
    static_assert(toUType(Later::Second) == 1);

    // Kad ti treba broj (npr. indeks u tuple), toUType ga daje bez ručnog tipa.
    std::tuple<std::string, std::string, int> info{"Ana", "ana@example.com", 42};
    std::cout << "  std::get<toUType(UserField::Email)>(info) = " << std::get<toUType(UserField::Email)>(info)
              << std::noboolalpha << "\n";
}

// ---------------------------------------------------------------- 5
void s05_union() {
    std::cout << "-- 5. union --\n";
    Number num;
    num.i = 42;                        // aktivan član: i
    std::cout << "  sizeof(Number)=" << sizeof(Number) << " (najveći član), num.i=" << num.i;
    num.f = 1.5f;                      // sada je aktivan f -- num.i više ne sme da se čita
    std::cout << ", posle num.f = 1.5f: num.f=" << num.f << "\n";

    // "Type punning" (čitanje bitova kao drugog tipa) preko union-a je UB u C++-u.
    // Ispravno: std::memcpy, ili std::bit_cast u C++20.
    float value = 1.0f;
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof bits);
    std::cout << "  bitovi od 1.0f preko memcpy: 0x" << std::hex << bits << std::dec;
#if __cplusplus >= 202002L
    std::cout << ", preko std::bit_cast: 0x" << std::hex << std::bit_cast<std::uint32_t>(value) << std::dec;
#endif
    std::cout << "\n";
}

// ---------------------------------------------------------------- 6
template <typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void s06_variant() {
    std::cout << "-- 6. std::variant (C++17) --\n";
    std::variant<int, std::string> v;  // podrazumevano: prva alternativa, int{} = 0
    std::cout << "  podrazumevano: index=" << v.index() << " vrednost=" << std::get<int>(v) << "\n";
    v = 42;
    std::cout << "  v = 42: holds int? " << std::boolalpha << std::holds_alternative<int>(v);
    v = std::string("tekst");
    std::cout << "; v = \"tekst\": holds int? " << std::holds_alternative<int>(v)
              << " get_if<int> == nullptr? " << (std::get_if<int>(&v) == nullptr) << std::noboolalpha << "\n";
    try {
        std::cout << std::get<int>(v);  // pogrešna alternativa: izuzetak, ne UB
    } catch (const std::bad_variant_access&) {
        std::cout << "  std::get<int> na string -> std::bad_variant_access\n";
    }
    auto print = overloaded{
        [](int i) { return "int " + std::to_string(i); },
        [](const std::string& s) { return "string \"" + s + "\""; },
    };
    std::cout << "  std::visit: " << std::visit(print, v) << "; sizeof(variant)=" << sizeof(v)
              << " > sizeof(std::string)=" << sizeof(std::string) << " (čuva i koji je aktivan)\n";
}

// ---------------------------------------------------------------- 7
typedef void (*OldHandler)(int, const std::string&);   // typedef: ime je usred deklaracije
using NewHandler = void (*)(int, const std::string&);  // using: ime levo, tip desno

template <typename T>
using MyList = std::vector<T>;                         // alias template -- typedef ovo ne može (errors/e13)

template <typename T>
struct MyListOld {                                     // stari način: typedef unutar struct-a
    typedef std::vector<T> type;
};

template <typename T>
struct Widget {
    MyList<T> modern;                                  // bez typename
    typename MyListOld<T>::type legacy;                // zavisni tip -> mora typename
};

void s07_aliases() {
    std::cout << "-- 7. using vs typedef (EMC Item 9) --\n";
    static_assert(std::is_same_v<OldHandler, NewHandler>);
    static_assert(std::is_same_v<MyList<int>, MyListOld<int>::type>);
    static_assert(std::is_same_v<std::remove_const_t<const int>, std::remove_const<const int>::type>);
    Widget<int> w;
    w.modern.push_back(1);
    w.legacy.push_back(2);
    std::cout << "  OldHandler == NewHandler, MyList<int> == MyListOld<int>::type (static_assert)\n";
}

// ---------------------------------------------------------------- 8
void onStart(int code) { std::cout << "  onStart(" << code << ")\n"; }
void onStop(int code) { std::cout << "  onStop(" << code << ")\n"; }

void s08_functionTypes() {
    std::cout << "-- 8. funkcijski tipovi --\n";
    using Handler = void(int);          // tip FUNKCIJE (nije pokazivač)
    static_assert(std::is_function_v<Handler>);
    Handler* ptr = onStart;             // pokazivač na funkciju (ime se raspada u pokazivač)
    Handler& ref = onStop;              // referenca na funkciju
    ptr(1);
    ref(2);
    Handler* table[] = {onStart, onStop};  // niz POKAZIVAČA na funkcije (niz funkcija ne postoji -- errors/e05)
    for (std::size_t i = 0; i < std::size(table); ++i) table[i](static_cast<int>(10 + i));
}

int main() {
    s01_typeCategories();
    s02_cArrays();
    s03_stdArray();
    s04_enums();
    s05_union();
    s06_variant();
    s07_aliases();
    s08_functionTypes();
}
