#include <functional>
#include <iostream>
#include <string>
#include <utility>

// Funkcije: overloading, default argumenti, pokazivači na funkcije --
// ISPRAVNI slučajevi. Sve se kompajlira i radi bez ASan/UBSan prijava
// (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/11-functions-advanced  proverava oba.
// inline i namespace su u lekciji 08.

// ---------------------------------------------------------------- 1
int area(int side) { return side * side; }
int area(int width, int height) { return width * height; }   // drugi broj parametara
double area(double radius) { return 3.14159 * radius * radius; } // drugi tip parametra

void s01_overloading() {
    std::cout << "-- 1. overloading: same name, different parameters --\n";
    std::cout << "  area(3)=" << area(3) << " area(2, 5)=" << area(2, 5) << " area(1.0)=" << area(1.0) << "\n";
    // Ne računa se: samo povratni tip (errors/e01), alias istog tipa
    // (errors/e02), const na parametru po vrednosti (lekcija 09, errors/e13).
}

// ---------------------------------------------------------------- 2
void f(int) { std::cout << "f(int)"; }
void f(double) { std::cout << "f(double)"; }

void g(bool) { std::cout << "g(bool)"; }
void g(const std::string&) { std::cout << "g(const std::string&)"; }

void s02_resolution() {
    std::cout << "-- 2. overload resolution: which overload wins --\n";
    char c = 'a';
    short s = 1;
    float fl = 1.5f;
    bool b = true;
    std::cout << "  f(char)  -> "; f(c);  std::cout << "   (promotion char -> int)\n";
    std::cout << "  f(short) -> "; f(s);  std::cout << "   (promotion short -> int)\n";
    std::cout << "  f(bool)  -> "; f(b);  std::cout << "   (promotion bool -> int)\n";
    std::cout << "  f(float) -> "; f(fl); std::cout << "   (promotion float -> double)\n";
    // f(5L) i f(long, double)+f(5) su dvosmisleni: obe strane su "konverzija" (errors/e03, e04).

    // Zamka: standardna konverzija (pokazivač -> bool) pobeđuje korisničku
    // konverziju (const char* -> std::string).
    std::cout << "  g(\"hello\") -> "; g("hello");
    std::cout << "   <- NOT string! const char* -> bool is a standard conversion\n";
    std::cout << "  g(std::string(\"hello\")) -> "; g(std::string("hello")); std::cout << "\n";
}

// ---------------------------------------------------------------- 3
void take(int&) { std::cout << "take(int&)"; }
void take(const int&) { std::cout << "take(const int&)"; }
void take(int&&) { std::cout << "take(int&&)"; }

void s03_referenceOverloads() {
    std::cout << "-- 3. overloading on reference kind --\n";
    int x = 1;
    const int cx = 2;
    std::cout << "  take(x)            -> "; take(x);            std::cout << "\n";
    std::cout << "  take(cx)           -> "; take(cx);           std::cout << "\n";
    std::cout << "  take(3)            -> "; take(3);            std::cout << "   (&& beats const& for an rvalue)\n";
    std::cout << "  take(std::move(x)) -> "; take(std::move(x)); std::cout << "\n";
}

// ---------------------------------------------------------------- 4
template <typename T>
void logName(T&&) { std::cout << "logName(T&&) -- template"; }
void logName(int) { std::cout << "logName(int)"; }

void s04_universalReferenceOverload() {
    std::cout << "-- 4. overloading on a universal reference (EMC Item 26) --\n";
    short idx = 1;
    std::cout << "  logName(1)     -> "; logName(1);   std::cout << "\n";
    std::cout << "  logName(short) -> "; logName(idx);
    std::cout << "   <- the template wins: T = short& is an EXACT match, int needs a promotion\n";
}

// ---------------------------------------------------------------- 5
bool isLucky(int number) { return number == 7; }
bool isLucky(char) = delete;   // EMC Item 11: zabrani neželjene konverzije
bool isLucky(double) = delete; // isLucky(3.5) -> greška, ne tihi isLucky(3) (errors/e10)

void s05_deletedOverloads() {
    std::cout << "-- 5. = delete na overload-u (EMC Item 11) --\n";
    std::cout << "  isLucky(7)=" << std::boolalpha << isLucky(7) << " isLucky(8)=" << isLucky(8)
              << std::noboolalpha << "  (isLucky('a') and isLucky(3.5) do not compile)\n";
}

// ---------------------------------------------------------------- 6
int nextId() {
    static int counter = 0;
    return ++counter;
}
void createUser(const std::string& name, int id = nextId()) { // računa se pri SVAKOM pozivu
    std::cout << "  createUser(" << name << ", id=" << id << ")\n";
}

enum Color { Red, Green };
struct Shape {
    virtual ~Shape() = default;
    virtual void draw(Color c = Red) const { std::cout << "Shape::draw(" << c << ")"; }
};
struct Circle : Shape {
    void draw(Color c = Green) const override { std::cout << "Circle::draw(color=" << c << ")"; }
};

void s06_defaultArguments() {
    std::cout << "-- 6. default arguments --\n";
    createUser("Ann");      // id = nextId() -> 1
    createUser("John");    // id = nextId() -> 2 (ponovo izračunato)
    createUser("Vera", 99);

    // EC++ Item 37: funkcija se bira DINAMIČKI (virtual), a podrazumevani
    // argument STATIČKI -- po tipu izraza, ne objekta.
    Circle circle;
    const Shape& asShape = circle;
    std::cout << "  asShape.draw() -> "; asShape.draw();
    std::cout << "   <- Circle::draw, but with Shape's Red (0)!\n";
    std::cout << "  circle.draw()  -> "; circle.draw(); std::cout << "\n";
}

// ---------------------------------------------------------------- 7
int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }
void process(int) { std::cout << "process(int)"; }
void process(double) { std::cout << "process(double)"; }

using BinaryOp = int (*)(int, int);  // čitljivije od "int (*op)(int, int)"

int apply(BinaryOp op, int a, int b) { return op(a, b); }

void s07_functionPointers() {
    std::cout << "-- 7. function pointers and callbacks --\n";
    BinaryOp op = add;   // ime funkcije se raspada u pokazivač (&add je isto)
    std::cout << "  apply(add, 2, 3)=" << apply(op, 2, 3) << " apply(mul, 2, 3)=" << apply(mul, 2, 3) << "\n";

    // Overload se bira po CILJNOM tipu (auto p = &process; ne radi -- errors/e08).
    void (*pd)(double) = process;
    auto pi = static_cast<void (*)(int)>(process);
    std::cout << "  "; pd(1.0); std::cout << ", "; pi(1); std::cout << "\n";

    // Lambda BEZ capture-a se pretvara u pokazivač na funkciju.
    BinaryOp sub = [](int a, int b) { return a - b; };
    std::cout << "  a lambda without captures as a pointer: apply(sub, 5, 3)=" << apply(sub, 5, 3) << "\n";

    // Lambda SA capture-om ne može (errors/e09) -- za nju je std::function.
    int factor = 10;
    std::function<int(int)> scale = [factor](int x) { return x * factor; };
    std::cout << "  std::function with a capture: scale(4)=" << scale(4) << "\n";

    // Prazan std::function baca izuzetak; prazan pokazivač na funkciju je UB (ub/u01).
    std::function<void()> empty;
    try {
        empty();
    } catch (const std::bad_function_call&) {
        std::cout << "  empty std::function -> std::bad_function_call (not UB)\n";
    }
}

int main() {
    s01_overloading();
    s02_resolution();
    s03_referenceOverloads();
    s04_universalReferenceOverload();
    s05_deletedOverloads();
    s06_defaultArguments();
    s07_functionPointers();
}
