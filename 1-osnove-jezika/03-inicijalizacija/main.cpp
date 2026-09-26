#include <atomic>
#include <initializer_list>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Inicijalizacija u C++ -- ISPRAVNI slučajevi. Sve u ovom fajlu se
// kompajlira i radi (g++ 13 i clang 18, -std=c++17 i -std=c++20).
// POGREŠNI slučajevi su u errors/: svaki fajl je jedan slučaj koji se NE
// kompajlira, a ./check_cases.sh <ova lekcija> proverava da svaki pada iz očekivanog
// razloga na oba kompajlera. Numeracija sekcija prati notes.md.
//
// Neki warning-i pri kompajliranju su NAMERNI (most vexing parse, redosled
// u init listi, izostavljeni članovi agregata) -- baš njih i demonstriramo.

// ---------------------------------------------------------------- 1
int g_global; // statičko trajanje: pre svega ide zero-initialization -> 0

struct A {
    int x;
};

void s01_defaultInit() {
    std::cout << "-- 1. default initialization --\n";
    static int s_local; // takođe statičko trajanje -> 0
    std::cout << "globalni int: " << g_global << ", static lokalni int: " << s_local << "\n";

    int x; // lokalni: NEODREĐENA vrednost -- čitanje pre upisa je UB
    A a;   // a.x takođe neodređen
    x = 1; // zato uvek prvo upis
    a.x = 2;
    std::cout << "lokalni posle upisa: x=" << x << " a.x=" << a.x << "\n";

    std::string s; // klasa sa default ctor-om: uvek ispravno inicijalizovana
    std::cout << "std::string s; -> size()=" << s.size() << " (prazan, ne garbage)\n";
}

// ---------------------------------------------------------------- 2
void s02_valueInit() {
    std::cout << "-- 2. value initialization --\n";
    int x{};       // 0
    int y = int(); // 0 -- stariji oblik istog
    double d{};    // 0.0
    bool b{};      // false
    int* p{};      // nullptr
    A a{};         // A je agregat -> formalno aggregate init, rezultat isti: a.x == 0
    std::cout << "int{}=" << x << " int()=" << y << " double{}=" << d
              << " bool{}=" << std::boolalpha << b << std::noboolalpha
              << " int*{}=" << (p == nullptr ? "nullptr" : "?")
              << " A{}.x=" << a.x << "\n";
}

// ---------------------------------------------------------------- 3
struct ExplicitOnly {
    explicit ExplicitOnly(int v) : v_(v) {}
    int v_;
};

struct Name {
    Name(std::string s) : v_(std::move(s)) {}
    std::string v_;
};

void s03_directInit() {
    std::cout << "-- 3. direct initialization --\n";
    int x(42);
    std::string s("hello");
    std::vector<int> v(10); // 10 elemenata, svi 0
    ExplicitOnly e(5);      // explicit ctor SE razmatra kod direct-init
    Name n("Marko");        // "Marko" -> std::string je JEDNA korisnička konverzija -- OK
    std::cout << "x=" << x << " s=" << s << " v.size()=" << v.size()
              << " e.v_=" << e.v_ << " n.v_=" << n.v_ << "\n";
}

// ---------------------------------------------------------------- 4
struct S {
    explicit S(int) { std::cout << "  -> explicit S(int)\n"; }
    S(long) { std::cout << "  -> S(long)\n"; }
};

void s04_copyInit() {
    std::cout << "-- 4. copy initialization --\n";
    int x = 42;
    std::string s = "hello"; // const char* -> std::string: jedna konverzija -- OK
    std::cout << "x=" << x << " s=" << s << "\n";

    // explicit S(int) se kod copy-init uopšte NE razmatra -> bira se S(long),
    // iako je S(int) tačniji match. (Name n = "Marko"; ne radi -- errors/e08.)
    std::cout << "S a = 1;\n";
    S a = 1;
    (void)a;

    // C++17 garantuje copy elision -> nekopirljiv tip radi i sa =.
    // U C++14 (kad je pisan EMC) ovo je bila greška -- errors/e23.
    std::atomic<int> counter = 0;
    std::cout << "std::atomic<int> counter = 0; -> " << counter.load() << "\n";
}

// ---------------------------------------------------------------- 5
void s05_listInit() {
    std::cout << "-- 5. list initialization {} --\n";
    int x{42};    // direct-list-init
    int y = {42}; // copy-list-init
    std::string s{"hello"};
    std::cout << "x=" << x << " y=" << y << " s=" << s << "\n";

    // direct-list-init razmatra explicit ctor -> explicit S(int) pobeđuje.
    // Isto sa "S c = {1};" je GREŠKA (errors/e07): i copy-list-init razmatra
    // explicit ctor, ali ako on pobedi, program je neispravan.
    std::cout << "S b{1};\n";
    S b{1};
    (void)b;
}

// ---------------------------------------------------------------- 6
void s06_narrowing() {
    std::cout << "-- 6. narrowing: () i = ga tiho puštaju, {} ga zabranjuje --\n";
    double d = 3.99;
    int a(d);  // 3 -- tiho odsecanje, BEZ warning-a čak i uz -Wall -Wextra
    int b = d; // 3 -- isto (upozorenje daje tek -Wconversion)
    std::cout << "int a(3.99) -> " << a << ", int b = 3.99 -> " << b << "\n";

    // {} pušta konverzije BEZ gubitka i konstante koje staju u ciljni tip:
    int c{3};         // int konstanta
    char ch{65};      // 65 staje u char
    float f{0.1};     // double konstanta u opsegu float-a -- dozvoljeno iako
                      // 0.1 nije tačno predstavljiv (pravilo za float -> float)
    double wide{f};   // float -> double: proširenje, nikad narrowing
    unsigned u{1};    // pozitivna konstanta
    long long big{c}; // int -> long long: proširenje
    std::cout << "int{3}=" << c << " char{65}=" << ch << " float{0.1}=" << f
              << " double{float}=" << wide << " unsigned{1}=" << u
              << " long long{int}=" << big << "\n";

    // Namerna konverzija sa gubitkom -- napiši je eksplicitno, da se vidi.
    int e{static_cast<int>(d)};
    std::cout << "int{static_cast<int>(3.99)} -> " << e << "\n";
    // Pogrešno: errors/e01 .. e05 (int{d}, int{3.14}, unsigned{-1},
    // float{double_promenljiva}, int{long_long_promenljiva}).
}

// ---------------------------------------------------------------- 7
class Person {
public:
    Person(std::string name, int age) : m_name{std::move(name)}, m_age{age} {}
    void print() const { std::cout << m_name << ", " << m_age << "\n"; }

private:
    std::string m_name;
    int m_age;
};

void s07_objects() {
    std::cout << "-- 7. {} za objekte --\n";
    Person p{"Marko", 30};
    p.print();
}

// ---------------------------------------------------------------- 8
class Config {
public:
    Config() = default;                                  // koristi default member initializere
    explicit Config(int timeout) : timeout_{timeout} {}  // init lista PREGAZI default za timeout_
    void print() const {
        std::cout << "timeout=" << timeout_ << " retries=" << retries_ << " name=" << name_ << "\n";
    }

private:
    int timeout_{5};
    int retries_ = 3; // i = radi; () ne radi -- errors/e09
    std::string name_{"default"};
};

void s08_defaultMemberInit() {
    std::cout << "-- 8. default member initializers --\n";
    Config a;
    Config b{60};
    std::cout << "Config a;     -> ";
    a.print();
    std::cout << "Config b{60}; -> ";
    b.print();
}

// ---------------------------------------------------------------- 9
struct Tracer {
    Tracer() { std::cout << "  Tracer() -- default ctor\n"; }
    Tracer(const char* s) { std::cout << "  Tracer(\"" << s << "\")\n"; }
    Tracer& operator=(const char* s) {
        std::cout << "  Tracer::operator=(\"" << s << "\")\n";
        return *this;
    }
};

struct ViaBody {
    ViaBody() { t = "x"; } // dodela: t je VEĆ default-konstruisan pre tela
    Tracer t;
};

struct ViaInitList {
    ViaInitList() : t{"x"} {} // prava inicijalizacija: jedan poziv
    Tracer t;
};

struct NoDefault {
    explicit NoDefault(int) {}
};

struct Mandatory {
    // Sva tri člana MORAJU biti u init listi (errors/e20, e21).
    Mandatory(int v, int& r) : c{v}, r_{r}, n{v} {}
    const int c;
    int& r_;
    NoDefault n; // tip bez default ctor-a
};

struct Named {
    explicit Named(const char* n) { std::cout << "  konstruisan " << n << "\n"; }
};

struct Order {
    // Init lista kaže a pa b, ali članovi se konstruišu redosledom
    // DEKLARACIJE: b pa a. -Wall (-Wreorder) upozorava na ovo.
    Order() : a{"a"}, b{"b"} {}
    Named b;
    Named a;
};

void s09_ctorInitList() {
    std::cout << "-- 9. constructor initializer list --\n";
    std::cout << "ViaBody (dodela u telu):\n";
    ViaBody vb;
    std::cout << "ViaInitList (init lista):\n";
    ViaInitList vil;
    int x = 7;
    Mandatory m(5, x);
    std::cout << "Mandatory: c=" << m.c << " r_=" << m.r_ << "\n";
    std::cout << "Order (init lista: a, b):\n";
    Order o;
    (void)vb;
    (void)vil;
    (void)o;
}

// ---------------------------------------------------------------- 10
void s10_stlTrap() {
    std::cout << "-- 10. STL zamka: () vs {} --\n";
    std::vector<int> v1(10);               // 10 elemenata, svi 0
    std::vector<int> v2{10};               // 1 element: 10
    std::vector<int> v3(10, 20);           // 10 elemenata, svi 20
    std::vector<int> v4{10, 20};           // 2 elementa: 10, 20
    std::vector<std::string> v5{10};       // 10 PRAZNIH stringova -- vidi sekciju 11
    std::vector<std::string> v6{"a", "b"}; // 2 elementa
    std::cout << "vector<int>(10)=" << v1.size() << " el.  vector<int>{10}=" << v2.size()
              << " el. (v2[0]=" << v2[0] << ")\n";
    std::cout << "vector<int>(10,20)=" << v3.size() << " el.  vector<int>{10,20}=" << v4.size() << " el.\n";
    std::cout << "vector<string>{10}=" << v5.size() << " el. (!)  vector<string>{\"a\",\"b\"}="
              << v6.size() << " el.\n";
}

// ---------------------------------------------------------------- 11
struct Basic {
    Basic(int, int) { std::cout << "  Basic(int, int)\n"; }
    Basic(std::initializer_list<int>) { std::cout << "  Basic(initializer_list<int>)\n"; }
};

class Widget {
public:
    Widget(int, bool) { std::cout << "  Widget(int, bool)\n"; }
    Widget(int, double) { std::cout << "  Widget(int, double)\n"; }
    Widget(std::initializer_list<long double> il) {
        std::cout << "  Widget(initializer_list<long double>), size=" << il.size() << "\n";
    }
    Widget(const Widget&) { std::cout << "  Widget(copy ctor)\n"; }
    Widget(Widget&&) noexcept { std::cout << "  Widget(move ctor)\n"; }
    operator float() const { return 0.0f; }
};

struct WidgetFallback {
    WidgetFallback(int, bool) { std::cout << "  WidgetFallback(int, bool)\n"; }
    WidgetFallback(std::initializer_list<std::string>) {
        std::cout << "  WidgetFallback(initializer_list<string>)\n";
    }
};

struct WidgetEmpty {
    WidgetEmpty() { std::cout << "  WidgetEmpty() -- default ctor\n"; }
    WidgetEmpty(std::initializer_list<int> il) {
        std::cout << "  WidgetEmpty(initializer_list<int>), size=" << il.size() << "\n";
    }
};

void s11_initializerListPriority() {
    std::cout << "-- 11. initializer_list ima prioritet (EMC Item 7) --\n";
    std::cout << "Basic a(1, 2); / Basic b{1, 2};\n";
    Basic a(1, 2);
    Basic b{1, 2};

    // Čak i kad postoji TAČAN match, {} bira initializer_list ctor ako se
    // argumenti mogu konvertovati u element-tip bez narrowing-a.
    std::cout << "Widget(10, true) / Widget{10, true} / Widget(10, 5.0) / Widget{10, 5.0}:\n";
    Widget w1(10, true);
    Widget w2{10, true};
    Widget w3(10, 5.0);
    Widget w4{10, 5.0};

    // Kompajler se vraća na obične konstruktore SAMO ako initializer_list
    // ctor uopšte nije moguć: int/bool ne mogu u std::string. Isti razlog
    // zašto je vector<string>{10} deset praznih stringova.
    std::cout << "WidgetFallback{10, true}:\n";
    WidgetFallback f{10, true};

    // Prazne {} znače "bez argumenata", NE prazna lista.
    std::cout << "WidgetEmpty e1{}; / e2({}); / e3{{}};\n";
    WidgetEmpty e1{};   // default ctor
    WidgetEmpty e2({}); // initializer_list ctor, PRAZNA lista (size=0)
    WidgetEmpty e3{{}}; // česta zabluda: NIJE prazna lista -- unutrašnje {} je
                        // JEDAN element (value-init int -> 0), size=1

    // ZAVISI OD KOMPAJLERA: {} sa objektom istog tipa kad klasa ima
    // initializer_list ctor I konverziju u element-tip (operator float).
    //   g++ 13:   initializer_list ctor, size=1  (po CWG 2137, važeći tekst standarda)
    //   clang 18: copy ctor / move ctor
    // Pouka: ne pravi klasu sa initializer_list ctor-om i konverzijom u
    // njegov element-tip, i za kopiju piši Widget w5(w4) sa zagradama.
    std::cout << "Widget w5{w4}; / Widget w6{std::move(w4)};  (zavisi od kompajlera):\n";
    Widget w5{w4};
    Widget w6{std::move(w4)};
    (void)w1; (void)w2; (void)w3; (void)w5; (void)w6; (void)f; (void)e1; (void)e2; (void)e3;
}

// ---------------------------------------------------------------- 12
void s12_auto() {
    std::cout << "-- 12. auto i {} --\n";
    auto a{5};          // int
    auto b = {5};       // std::initializer_list<int>
    auto c = 5;         // int
    auto d = {1, 2, 3}; // std::initializer_list<int>
    static_assert(std::is_same_v<decltype(a), int>);
    static_assert(std::is_same_v<decltype(b), std::initializer_list<int>>);
    static_assert(std::is_same_v<decltype(c), int>);
    static_assert(std::is_same_v<decltype(d), std::initializer_list<int>>);
    std::cout << "auto a{5} -> int, auto b = {5} -> initializer_list<int>, "
              << "auto d = {1,2,3} -> initializer_list<int> (proverava static_assert)\n";
    (void)a; (void)b; (void)c; (void)d;
}

// ---------------------------------------------------------------- 13
void s13_dynamic() {
    std::cout << "-- 13. dinamička alokacija --\n";
    int* p1 = new int(42);
    int* p2 = new int{42};
    int* p3 = new int;   // default-init: NEODREĐENA vrednost
    int* p4 = new int(); // value-init: 0 -- () ovde NIJE most vexing parse
    int* p5 = new int{}; // value-init: 0
    *p3 = 7;             // pre čitanja mora upis
    int* arr1 = new int[5]{};     // svi 0
    int* arr2 = new int[5]{1, 2}; // 1 2 0 0 0
    Person* person = new Person{"Marko", 30};
    std::cout << "new int(42)=" << *p1 << " new int{42}=" << *p2 << " new int()=" << *p4
              << " new int{}=" << *p5 << "\nnew int[5]{1,2} = ";
    for (int i = 0; i < 5; ++i) std::cout << arr2[i] << ' ';
    std::cout << "  new int[5]{} = ";
    for (int i = 0; i < 5; ++i) std::cout << arr1[i] << ' ';
    std::cout << "\nnew Person{\"Marko\", 30} -> ";
    person->print();
    delete p1; delete p2; delete p3; delete p4; delete p5;
    delete[] arr1; delete[] arr2;
    delete person;
}

// ---------------------------------------------------------------- 14
struct Point {
    int x;
    int y;
};

struct Line {
    Point a;
    Point b;
};

void s14_aggregate() {
    std::cout << "-- 14. aggregate initialization --\n";
    Point p{1, 2};
    Point q{1};  // y = 0 (dozvoljeno; -Wextra ipak upozori na izostavljen član)
    Point z{};   // oba 0
    Line l{{0, 0}, {3, 4}}; // ugnežđeni agregati
    int arr[]{1, 2, 3, 4};  // veličina (4) se dedukuje
    int part[5]{1, 2};      // 1 2 0 0 0
    std::cout << "Point{1,2}=(" << p.x << "," << p.y << ") Point{1}=(" << q.x << "," << q.y
              << ") Point{}=(" << z.x << "," << z.y << ") Line.b=(" << l.b.x << "," << l.b.y << ")\n";
    std::cout << "sizeof(arr)/sizeof(int)=" << sizeof(arr) / sizeof(int) << "  int[5]{1,2} = ";
    for (int v : part) std::cout << v << ' ';
    std::cout << "\n";
#if __cplusplus < 202002L
    // C++17: struktura sa "= default" ctor-om JOŠ JE agregat (ctor nije
    // user-provided). U C++20 više nije (user-declared) -- errors/e22.
    struct Defaulted {
        Defaulted() = default;
        int x;
        int y;
    };
    Defaulted dd{1, 2};
    std::cout << "C++17: Defaulted{1,2} (ima = default ctor) je agregat: (" << dd.x << "," << dd.y << ")\n";
#else
    // C++20 (P0960): agregat može i sa (). U C++17 greška -- errors/e16.
    Point paren(1, 2);
    std::cout << "C++20: Point(1, 2) = (" << paren.x << "," << paren.y << ")\n";
#endif
}

// ---------------------------------------------------------------- 15
#if __cplusplus >= 202002L
struct NetConfig {
    int timeout;
    int retries;
    bool verbose;
};

void s15_designated() {
    std::cout << "-- 15. designated initializers (C++20) --\n";
    NetConfig a{.timeout = 10, .retries = 3}; // verbose -> false
    NetConfig b{.retries = 5};                // preskakanje je dozvoljeno: timeout -> 0
    std::cout << "a: timeout=" << a.timeout << " retries=" << a.retries << " verbose=" << a.verbose << "\n";
    std::cout << "b: timeout=" << b.timeout << " retries=" << b.retries << " verbose=" << b.verbose << "\n";
    // Pogrešno: pogrešan redosled (errors/e17), mešanje sa pozicionim (errors/e18).
}
#else
void s15_designated() {
    std::cout << "-- 15. designated initializers (C++20) -- PRESKOČENO, pokreni sa -std=c++20 --\n";
}
#endif

// ---------------------------------------------------------------- 16
struct MyClass {
    int value = 0;
};
struct Timer {};
struct TimerWidget {
    explicit TimerWidget(Timer) {}
};

void s16_mostVexingParse() {
    std::cout << "-- 16. most vexing parse --\n";
    MyClass obj(); // deklaracija FUNKCIJE obj: bez parametara, vraća MyClass
    static_assert(std::is_function_v<decltype(obj)>);

    TimerWidget w(Timer()); // "pravi" most vexing parse: funkcija w čiji je
                            // parametar funkcija koja vraća Timer
    static_assert(std::is_function_v<decltype(w)>);

    MyClass ok1{};            // objekat
    MyClass ok2;              // objekat
    TimerWidget ok3{Timer{}}; // objekat
    TimerWidget ok4((Timer())); // objekat -- i dodatne zagrade rešavaju
    std::cout << "obj i w su funkcije (static_assert); ok1.value=" << ok1.value
              << " ok2.value=" << ok2.value << "\n";
    (void)ok3;
    (void)ok4;
}

// ---------------------------------------------------------------- 17
// Autor generičke funkcije ne zna da li pozivalac očekuje () ili {}
// ponašanje -- std::make_unique/std::make_shared zato koriste () i to je
// deo njihove dokumentacije (EMC Item 7, Item 21).
template <typename T, typename... Ts>
T makeWithParens(Ts&&... params) {
    return T(std::forward<Ts>(params)...);
}

template <typename T, typename... Ts>
T makeWithBraces(Ts&&... params) {
    return T{std::forward<Ts>(params)...};
}

void s17_genericCode() {
    std::cout << "-- 17. generički kod: () vs {} za isti poziv --\n";
    auto v1 = makeWithParens<std::vector<int>>(10, 20);
    auto v2 = makeWithBraces<std::vector<int>>(10, 20);
    std::cout << "makeWithParens<vector<int>>(10, 20) -> " << v1.size()
              << " el.   makeWithBraces<vector<int>>(10, 20) -> " << v2.size() << " el.\n";
}

int main() {
    s01_defaultInit();
    s02_valueInit();
    s03_directInit();
    s04_copyInit();
    s05_listInit();
    s06_narrowing();
    s07_objects();
    s08_defaultMemberInit();
    s09_ctorInitList();
    s10_stlTrap();
    s11_initializerListPriority();
    s12_auto();
    s13_dynamic();
    s14_aggregate();
    s15_designated();
    s16_mostVexingParse();
    s17_genericCode();
}
