#include <any>
#include <cstdlib>
#include <iostream>
#include <map>
#include <new>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

// std::optional, std::variant, std::any (C++17) -- ISPRAVNI slučajevi.
// Sve se kompajlira bez upozorenja i radi bez ASan/UBSan prijava (g++ 13
// i clang 18, C++17 i C++20). Brojevi sekcija prate notes.md. sizeof i
// broj alokacija su za libstdc++ na x86-64.
// POGREŠNI slučajevi:
//   errors/   -- kod koji se NE kompajlira
//   ub/       -- * na praznom optional-u, get_if bez provere
//   runtime/  -- value(), get<>, any_cast na pogrešnom sadržaju, bez try
// ./check_cases.sh 7-standard-library/37-optional-variant-any  proverava sve.

static int allocations = 0;   // brojač za sekcije 2 i 7
void* operator new(std::size_t n) {
    ++allocations;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

// ---------------------------------------------------------------- 1
std::optional<int> parse(const std::string& s) {
    if (s.empty() || s.find_first_not_of("0123456789") != std::string::npos) return std::nullopt;
    return std::stoi(s);                                // int -> optional<int> implicitno
}

void section1() {
    std::cout << "\n== 1. std::optional: a value or nothing\n";
    for (const char* input : {"42", "4x2", ""}) {
        std::optional<int> r = parse(input);
        std::cout << '"' << input << "\": ";
        if (r)                                          // isto što i r.has_value()
            std::cout << "number " << *r << '\n';         // * ne proverava (ub/u01)
        else
            std::cout << "not a number\n";
    }
    std::cout << "value_or: " << parse("x").value_or(-1) << '\n';
    try {
        parse("x").value();                          // value() proverava i baca
    } catch (const std::bad_optional_access&) {
        std::cout << "value() on an empty one: bad_optional_access\n";
    }
}

// ---------------------------------------------------------------- 2
struct Point {
    int x, y;
    Point(int a, int b) : x(a), y(b) {}                 // nema podrazumevani konstruktor
};

void section2() {
    std::cout << "\n== 2. optional: creation, modification, cost\n";
    std::optional<Point> t;                             // prazan -- Point se NE pravi
    std::cout << "empty: " << t.has_value() << '\n';
    t.emplace(3, 4);                                    // napravi na mestu
    std::cout << "after emplace: (" << t->x << ", " << t->y << ")\n";
    std::optional<Point> u(std::in_place, 5, 6);        // na mestu, već u konstruktoru
    t = std::nullopt;                                   // isprazni (isto: t.reset())
    std::cout << "after = nullopt: " << t.has_value() << ", u.x = " << u->x << '\n';
    auto m = std::make_optional<std::string>(3, 'a');   // "aaa"
    std::cout << "make_optional<string>(3, 'a'): " << *m << '\n';

    std::cout << "sizeof: int " << sizeof(int) << ", optional<int> " << sizeof(std::optional<int>)
              << ", optional<double> " << sizeof(std::optional<double>) << '\n';
    allocations = 0;
    std::optional<std::vector<int>> empty;
    std::optional<long> number = 7;
    std::cout << "allocations for an empty optional<vector> and optional<long>: " << allocations << " (no heap)\n";
    (void)empty;
    (void)number;
}

// ---------------------------------------------------------------- 3
class Sensor {
public:
    // Lenja inicijalizacija: kalibracija se računa pri prvom čitanju.
    double calibration() {
        if (!cal_) cal_ = compute();
        return *cal_;
    }
    int computations() const { return computations_; }

private:
    double compute() {
        ++computations_;
        return 1.25;
    }
    std::optional<double> cal_;
    int computations_ = 0;
};

void section3() {
    std::cout << "\n== 3. optional in practice\n";
    Sensor s;
    s.calibration();
    s.calibration();
    std::cout << "lazy initialization: 2 reads, " << s.computations() << " computation\n";

    std::optional<int> empty, zero = 0;
    std::cout << "comparison: empty == nullopt " << (empty == std::nullopt) << ", empty < 0 " << (empty < 0)
              << ", zero == 0 " << (zero == 0) << '\n';

    std::optional<bool> closed = false;              // IMA vrednost, i ona je false
    std::cout << "optional<bool> = false: if (o) -> " << static_cast<bool>(closed) << ", *o -> " << *closed
              << " (exercise ex2)\n";
}

// ---------------------------------------------------------------- 4
void section4() {
    std::cout << "\n== 4. std::variant: one of several types\n";
    std::variant<int, double, std::string> v;           // podrazumevano: prva alternativa, int{}
    std::cout << "default: index " << v.index() << ", int " << std::get<int>(v) << '\n';
    v = 2.5;
    std::cout << "after = 2.5: index " << v.index() << ", holds double " << std::holds_alternative<double>(v)
              << '\n';
    v = std::string("temp");
    std::cout << "after = string: get<2> " << std::get<2>(v) << '\n';
    if (auto* d = std::get_if<double>(&v))              // pokazivač ili nullptr -- bez izuzetka
        std::cout << "double " << *d << '\n';
    else
        std::cout << "get_if<double>: nullptr\n";
    try {
        std::get<int>(v);
    } catch (const std::bad_variant_access&) {
        std::cout << "get<int> while it holds a string: bad_variant_access\n";
    }
    std::cout << "sizeof variant<int, double>: " << sizeof(std::variant<int, double>) << '\n';

    struct NoDefault {
        explicit NoDefault(int) {}
    };
    std::variant<std::monostate, NoDefault> m;  // monostate: "prazno" stanje (errors/e07)
    std::cout << "variant<monostate, ...>: index " << m.index() << '\n';
}

// ---------------------------------------------------------------- 5
template <typename... F>
struct Overloaded : F... {
    using F::operator()...;                             // C++17: using sa paketom
};
template <typename... F>
Overloaded(F...) -> Overloaded<F...>;               // C++17 CTAD vodič (lekcija 29); C++20 ne treba

using Message = std::variant<int, double, std::string>;

void section5() {
    std::cout << "\n== 5. std::visit\n";
    std::vector<Message> messages{42, 3.5, std::string("alarm")};
    for (const auto& p : messages)
        std::visit(Overloaded{
                       [](int i) { std::cout << "int " << i << '\n'; },
                       [](double d) { std::cout << "double " << d << '\n'; },
                       [](const std::string& s) { std::cout << "string " << s << '\n'; },
                   },
                   p);

    // Generička lambda + if constexpr (lekcija 29) -- jedna lambda za sve tipove.
    for (const auto& p : messages) {
        std::string description = std::visit(
            [](const auto& x) -> std::string {
                using T = std::decay_t<decltype(x)>;
                if constexpr (std::is_same_v<T, std::string>)
                    return "text of length " + std::to_string(x.size());
                else
                    return "number " + std::to_string(static_cast<int>(x));
            },
            p);
        std::cout << description << '\n';
    }
}

// ---------------------------------------------------------------- 6
struct Off {};
struct Running {
    int speed;
};
struct Fault {
    std::string description;
};
using State = std::variant<Off, Running, Fault>;

State next(const State& s, const std::string& event) {
    return std::visit(Overloaded{
                          [&](const Off&) -> State {
                              if (event == "start") return Running{1};
                              return Off{};
                          },
                          [&](const Running& r) -> State {
                              if (event == "faster") return Running{r.speed + 1};
                              if (event == "fault") return Fault{"overheated at speed " + std::to_string(r.speed)};
                              if (event == "stop") return Off{};
                              return r;
                          },
                          [&](const Fault& g) -> State {
                              if (event == "reset") return Off{};
                              return g;
                          },
                      },
                      s);
}

struct ThrowingCopy {
    ThrowingCopy() = default;
    ThrowingCopy(const ThrowingCopy&) { throw std::runtime_error("copy throws"); }
    ThrowingCopy& operator=(const ThrowingCopy&) = default;
};

void section6() {
    std::cout << "\n== 6. variant as a state machine\n";
    const char* names[] = {"Off", "Running", "Fault"};
    State s = Off{};
    for (const char* d : {"start", "faster", "faster", "fault", "faster", "reset"}) {
        s = next(s, d);
        std::cout << d << " -> " << names[s.index()];
        if (auto* r = std::get_if<Running>(&s)) std::cout << " (speed " << r->speed << ')';
        if (auto* g = std::get_if<Fault>(&s)) std::cout << " (" << g->description << ')';
        std::cout << '\n';
    }

    std::variant<int, ThrowingCopy> v = 5;
    ThrowingCopy source;
    try {
        v = source;                                      // stari int uništen, kopija baci -- nema ni jednog ni drugog
    } catch (const std::exception&) {
    }
    std::cout << "valueless_by_exception: " << v.valueless_by_exception()
              << ", index == variant_npos: " << (v.index() == std::variant_npos) << '\n';
}

// ---------------------------------------------------------------- 7
struct Big {
    char bytes[64];
};

void section7() {
    std::cout << "\n== 7. std::any\n";
    std::map<std::string, std::any> properties;
    properties["id"] = 7;
    properties["name"] = std::string("temp");
    properties["active"] = true;
    std::cout << "id " << std::any_cast<int>(properties["id"]) << ", name " << std::any_cast<std::string>(properties["name"])
              << '\n';
    std::cout << "type() == typeid(bool): " << (properties["active"].type() == typeid(bool)) << '\n';
    if (auto* p = std::any_cast<double>(&properties["id"]))   // pokazivač: nullptr ako tip nije tačan
        std::cout << *p << '\n';
    else
        std::cout << "any_cast<double>(&a): nullptr\n";
    try {
        std::any_cast<double>(properties["id"]);
    } catch (const std::bad_any_cast&) {
        std::cout << "any_cast<double> on an int: bad_any_cast\n";
    }
    std::any literal = "abc";                           // const char*, ne std::string (zadatak ex3)
    std::cout << "\"abc\" in any: const char* " << (literal.type() == typeid(const char*)) << '\n';

    allocations = 0;
    std::any small = 5;
    int forSmall = allocations;
    allocations = 0;
    std::any big = Big{};
    std::cout << "allocations: any with int " << forSmall << ", any with 64 B " << allocations << '\n';
    std::any empty;
    std::cout << "sizeof(any) " << sizeof(std::any) << ", empty.has_value() " << empty.has_value() << '\n';
    (void)small;
    (void)big;
}

int main() {
    std::cout << std::boolalpha;
    section1();
    section2();
    section3();
    section4();
    section5();
    section6();
    section7();
}
