// Korak 1 -- demonstracije uz notes.md (svaka sekcija "// ----- N" prati
// sekciju N u notes.md).
//   ./build.sh roadmap/step-1-c-to-cpp/demos.cpp
// Za greške kompajlera: otkomentariši red označen sa "ne kompajlira se".

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <stdexcept>

// ----- 1. RAII: resurs = objekat, cleanup = destruktor

class Tracer {
public:
    explicit Tracer(const char* name) : name_{name} { std::printf("  acquire %s\n", name_); }
    ~Tracer() { std::printf("  release %s\n", name_); }

    Tracer(const Tracer&) = delete;             // dva vlasnika istog resursa = bug
    Tracer& operator=(const Tracer&) = delete;

private:
    const char* name_;
};

int early_return(bool fail) {
    Tracer a{"a"};
    Tracer b{"b"};
    if (fail) {
        std::printf("  early return\n");
        return -1;                               // b pa a, bez ijednog free()
    }
    std::printf("  normal path\n");
    return 0;
}

void throws() {
    Tracer c{"c"};
    throw std::runtime_error{"boom"};            // c se oslobađa tokom stack unwinding-a
}

// ----- 2. Reference vs pokazivači

void inc_ref(int& x) { ++x; }                    // ne može null, nema provere
void inc_ptr(int* x) {
    if (x != nullptr) ++*x;                      // može null, MORA provera
}
std::size_t length(const char* s) {              // C stil: pokazivač, može null
    std::size_t n = 0;
    while (s != nullptr && s[n] != '\0') ++n;
    return n;
}

// const int& dangling() { int local = 42; return local; }  // UB: vraća referencu na mrtav objekat
//                                                          // (g++: -Wreturn-local-addr)

// ----- 3. nullptr, enum class, constexpr

void f(int) { std::printf("  f(int)\n"); }
void f(int*) { std::printf("  f(int*)\n"); }

enum class Led : std::uint8_t { Off, Red, Green };
enum class Motor : std::uint8_t { Off, Forward, Reverse };  // isto ime "Off", nema kolizije

constexpr std::uint32_t kBaud = 115200;
constexpr std::uint32_t kClock = 16'000'000;
constexpr std::uint32_t divisor(std::uint32_t clock, std::uint32_t baud) { return clock / (16 * baud); }
static_assert(divisor(kClock, kBaud) == 8, "pogrešan delilac za UART");  // provera pri kompajliranju

// ----- 4. const korektnost

class Counter {
public:
    int value() const { return value_; }         // sme na const objektu
    void increment() { ++value_; }               // ne sme na const objektu

private:
    int value_ = 0;
};

void print_counter(const Counter& c) {
    std::printf("  counter = %d\n", c.value());
    // c.increment();                            // ne kompajlira se: increment() nije const
}

// ----- 5. Preopterećenje funkcija i operatora

struct Vec2 {
    int x;
    int y;
};
Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
bool operator==(Vec2 a, Vec2 b) { return a.x == b.x && a.y == b.y; }

void show(int v) { std::printf("  show(int) %d\n", v); }
void show(double v) { std::printf("  show(double) %.1f\n", v); }
void show(Vec2 v) { std::printf("  show(Vec2) {%d, %d}\n", v.x, v.y); }

// ----- 6. Imenski prostori

namespace uart {
constexpr int kPort = 1;
void init() { std::printf("  uart::init port %d\n", kPort); }
}  // namespace uart

namespace spi {
void init() { std::printf("  spi::init\n"); }  // isto ime, nema kolizije
}  // namespace spi

namespace {
int helper_calls = 0;                            // internal linkage, kao "static" u C-u
}

// ----- 7. struct vs class

struct Point {                                   // pasivni podaci, sve javno
    int x = 0;
    int y = 0;
};

class Percent {                                  // ima invarijantu: 0..100
public:
    explicit Percent(int v) : v_{v < 0 ? 0 : (v > 100 ? 100 : v)} {}
    int get() const { return v_; }

private:
    int v_;                                      // privatno: niko spolja ne može da pokvari invarijantu
};

// ----- 8. Inicijalizacija

struct Logger {
    Logger() { std::printf("  Logger() -- konstruktor\n"); }
    Logger(const Logger&) { std::printf("  Logger(const Logger&) -- copy konstruktor\n"); }
    Logger& operator=(const Logger&) {
        std::printf("  operator= -- dodela\n");
        return *this;
    }
};

int main() {
    std::printf("== 1. RAII\n");
    early_return(true);
    early_return(false);
    try {
        throws();
    } catch (const std::exception& e) {
        std::printf("  caught: %s\n", e.what());
    }

    std::printf("== 2. references vs pointers\n");
    int n = 1;
    inc_ref(n);
    inc_ptr(&n);
    inc_ptr(nullptr);
    std::printf("  n = %d, length(nullptr) = %zu, length(\"abc\") = %zu\n", n, length(nullptr), length("abc"));
    int other = 100;
    int& r = n;
    r = other;                                   // NE prevezuje r; upisuje 100 u n
    std::printf("  after r = other: n = %d\n", n);

    std::printf("== 3. nullptr, enum class, constexpr\n");
    f(0);                                        // 0 je int
    f(nullptr);                                  // nullptr nije int -- bira f(int*)
    // f(NULL);                                  // g++: dvosmisleno ili f(int) -- zavisi od implementacije
    Led led = Led::Green;
    // int i = led;                              // ne kompajlira se: nema implicitne konverzije
    std::printf("  led = %d (eksplicitno static_cast)\n", static_cast<int>(led));
    std::printf("  UART divisor = %u (izračunato pri kompajliranju)\n", divisor(kClock, kBaud));

    std::printf("== 4. const\n");
    Counter c;
    c.increment();
    print_counter(c);
    int x = 1;
    int y = 2;
    const int* p1 = &x;                          // ne menjam *p1, mogu da menjam p1
    p1 = &y;
    int* const p2 = &x;                          // mogu da menjam *p2, ne mogu p2
    *p2 = 10;
    std::printf("  *p1 = %d, *p2 = %d\n", *p1, *p2);

    std::printf("== 5. overloading\n");
    show(1);
    show(1.5);
    show(Vec2{1, 2} + Vec2{3, 4});
    std::printf("  {1,2} == {1,2}: %d\n", Vec2{1, 2} == Vec2{1, 2});

    std::printf("== 6. namespaces\n");
    uart::init();
    spi::init();
    using uart::init;                            // using-deklaracija: samo ovo jedno ime
    init();
    ++helper_calls;
    std::printf("  helper_calls = %d\n", helper_calls);

    std::printf("== 7. struct vs class\n");
    Point pt{3, 4};
    pt.x = 5;
    Percent pc{150};
    std::printf("  Point {%d, %d}, Percent(150) = %d\n", pt.x, pt.y, pc.get());

    std::printf("== 8. initialization\n");
    int a1;                                      // default-init: neodređena vrednost (ne čitaj!)
    int a2{};                                    // value-init: 0
    // int a3();                                 // NIJE promenljiva: deklaracija funkcije (most vexing parse)
    a1 = 7;
    std::printf("  a1 = %d, a2 = %d\n", a1, a2);
    Logger l1;
    Logger l2 = l1;                              // inicijalizacija: copy konstruktor, NE operator=
    l2 = l1;                                     // dodela postojećem objektu: operator=
}

/* EXPECTED OUTPUT
== 1. RAII
  acquire a
  acquire b
  early return
  release b
  release a
  acquire a
  acquire b
  normal path
  release b
  release a
  acquire c
  release c
  caught: boom
== 2. references vs pointers
  n = 3, length(nullptr) = 0, length("abc") = 3
  after r = other: n = 100
== 3. nullptr, enum class, constexpr
  f(int)
  f(int*)
  led = 2 (eksplicitno static_cast)
  UART divisor = 8 (izračunato pri kompajliranju)
== 4. const
  counter = 1
  *p1 = 2, *p2 = 10
== 5. overloading
  show(int) 1
  show(double) 1.5
  show(Vec2) {4, 6}
  {1,2} == {1,2}: 1
== 6. namespaces
  uart::init port 1
  spi::init
  uart::init port 1
  helper_calls = 1
== 7. struct vs class
  Point {5, 4}, Percent(150) = 100
== 8. initialization
  a1 = 7, a2 = 0
  Logger() -- konstruktor
  Logger(const Logger&) -- copy konstruktor
  operator= -- dodela
*/
