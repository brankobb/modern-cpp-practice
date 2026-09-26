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
// ./check_cases.sh 7-standardna-biblioteka/37-optional-variant-any  proverava sve.

static int alokacija = 0;   // brojač za sekcije 2 i 7
void* operator new(std::size_t n) {
    ++alokacija;
    if (void* p = std::malloc(n)) return p;
    throw std::bad_alloc();
}
void operator delete(void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }

// ---------------------------------------------------------------- 1
std::optional<int> parsiraj(const std::string& s) {
    if (s.empty() || s.find_first_not_of("0123456789") != std::string::npos) return std::nullopt;
    return std::stoi(s);                                // int -> optional<int> implicitno
}

void sekcija1() {
    std::cout << "\n== 1. std::optional: vrednost ili ništa\n";
    for (const char* ulaz : {"42", "4x2", ""}) {
        std::optional<int> r = parsiraj(ulaz);
        std::cout << '"' << ulaz << "\": ";
        if (r)                                          // isto što i r.has_value()
            std::cout << "broj " << *r << '\n';         // * ne proverava (ub/u01)
        else
            std::cout << "nije broj\n";
    }
    std::cout << "value_or: " << parsiraj("x").value_or(-1) << '\n';
    try {
        parsiraj("x").value();                          // value() proverava i baca
    } catch (const std::bad_optional_access&) {
        std::cout << "value() na praznom: bad_optional_access\n";
    }
}

// ---------------------------------------------------------------- 2
struct Tacka {
    int x, y;
    Tacka(int a, int b) : x(a), y(b) {}                 // nema podrazumevani konstruktor
};

void sekcija2() {
    std::cout << "\n== 2. optional: pravljenje, izmena, cena\n";
    std::optional<Tacka> t;                             // prazan -- Tacka se NE pravi
    std::cout << "prazan: " << t.has_value() << '\n';
    t.emplace(3, 4);                                    // napravi na mestu
    std::cout << "posle emplace: (" << t->x << ", " << t->y << ")\n";
    std::optional<Tacka> u(std::in_place, 5, 6);        // na mestu, već u konstruktoru
    t = std::nullopt;                                   // isprazni (isto: t.reset())
    std::cout << "posle = nullopt: " << t.has_value() << ", u.x = " << u->x << '\n';
    auto m = std::make_optional<std::string>(3, 'a');   // "aaa"
    std::cout << "make_optional<string>(3, 'a'): " << *m << '\n';

    std::cout << "sizeof: int " << sizeof(int) << ", optional<int> " << sizeof(std::optional<int>)
              << ", optional<double> " << sizeof(std::optional<double>) << '\n';
    alokacija = 0;
    std::optional<std::vector<int>> prazan;
    std::optional<long> broj = 7;
    std::cout << "alokacija za prazan optional<vector> i optional<long>: " << alokacija << " (bez heap-a)\n";
    (void)prazan;
    (void)broj;
}

// ---------------------------------------------------------------- 3
class Senzor {
public:
    // Lenja inicijalizacija: kalibracija se računa pri prvom čitanju.
    double kalibracija() {
        if (!kal_) kal_ = izracunaj();
        return *kal_;
    }
    int racunanja() const { return racunanja_; }

private:
    double izracunaj() {
        ++racunanja_;
        return 1.25;
    }
    std::optional<double> kal_;
    int racunanja_ = 0;
};

void sekcija3() {
    std::cout << "\n== 3. optional u praksi\n";
    Senzor s;
    s.kalibracija();
    s.kalibracija();
    std::cout << "lenja inicijalizacija: 2 čitanja, " << s.racunanja() << " računanje\n";

    std::optional<int> prazan, nula = 0;
    std::cout << "poređenje: prazan == nullopt " << (prazan == std::nullopt) << ", prazan < 0 " << (prazan < 0)
              << ", nula == 0 " << (nula == 0) << '\n';

    std::optional<bool> zatvoreno = false;              // IMA vrednost, i ona je false
    std::cout << "optional<bool> = false: if (o) -> " << static_cast<bool>(zatvoreno) << ", *o -> " << *zatvoreno
              << " (zadatak z2)\n";
}

// ---------------------------------------------------------------- 4
void sekcija4() {
    std::cout << "\n== 4. std::variant: jedno od nekoliko tipova\n";
    std::variant<int, double, std::string> v;           // podrazumevano: prva alternativa, int{}
    std::cout << "podrazumevano: index " << v.index() << ", int " << std::get<int>(v) << '\n';
    v = 2.5;
    std::cout << "posle = 2.5: index " << v.index() << ", holds double " << std::holds_alternative<double>(v)
              << '\n';
    v = std::string("temp");
    std::cout << "posle = string: get<2> " << std::get<2>(v) << '\n';
    if (auto* d = std::get_if<double>(&v))              // pokazivač ili nullptr -- bez izuzetka
        std::cout << "double " << *d << '\n';
    else
        std::cout << "get_if<double>: nullptr\n";
    try {
        std::get<int>(v);
    } catch (const std::bad_variant_access&) {
        std::cout << "get<int> dok drži string: bad_variant_access\n";
    }
    std::cout << "sizeof variant<int, double>: " << sizeof(std::variant<int, double>) << '\n';

    struct BezPodrazumevanog {
        explicit BezPodrazumevanog(int) {}
    };
    std::variant<std::monostate, BezPodrazumevanog> m;  // monostate: "prazno" stanje (errors/e07)
    std::cout << "variant<monostate, ...>: index " << m.index() << '\n';
}

// ---------------------------------------------------------------- 5
template <typename... F>
struct Preopterecen : F... {
    using F::operator()...;                             // C++17: using sa paketom
};
template <typename... F>
Preopterecen(F...) -> Preopterecen<F...>;               // C++17 CTAD vodič (lekcija 29); C++20 ne treba

using Poruka = std::variant<int, double, std::string>;

void sekcija5() {
    std::cout << "\n== 5. std::visit\n";
    std::vector<Poruka> poruke{42, 3.5, std::string("alarm")};
    for (const auto& p : poruke)
        std::visit(Preopterecen{
                       [](int i) { std::cout << "int " << i << '\n'; },
                       [](double d) { std::cout << "double " << d << '\n'; },
                       [](const std::string& s) { std::cout << "string " << s << '\n'; },
                   },
                   p);

    // Generička lambda + if constexpr (lekcija 29) -- jedna lambda za sve tipove.
    for (const auto& p : poruke) {
        std::string opis = std::visit(
            [](const auto& x) -> std::string {
                using T = std::decay_t<decltype(x)>;
                if constexpr (std::is_same_v<T, std::string>)
                    return "tekst dužine " + std::to_string(x.size());
                else
                    return "broj " + std::to_string(static_cast<int>(x));
            },
            p);
        std::cout << opis << '\n';
    }
}

// ---------------------------------------------------------------- 6
struct Ugasen {};
struct Radi {
    int brzina;
};
struct Greska {
    std::string opis;
};
using Stanje = std::variant<Ugasen, Radi, Greska>;

Stanje sledece(const Stanje& s, const std::string& dogadjaj) {
    return std::visit(Preopterecen{
                          [&](const Ugasen&) -> Stanje {
                              if (dogadjaj == "start") return Radi{1};
                              return Ugasen{};
                          },
                          [&](const Radi& r) -> Stanje {
                              if (dogadjaj == "brze") return Radi{r.brzina + 1};
                              if (dogadjaj == "kvar") return Greska{"pregrejan na brzini " + std::to_string(r.brzina)};
                              if (dogadjaj == "stop") return Ugasen{};
                              return r;
                          },
                          [&](const Greska& g) -> Stanje {
                              if (dogadjaj == "reset") return Ugasen{};
                              return g;
                          },
                      },
                      s);
}

struct KopijaBaca {
    KopijaBaca() = default;
    KopijaBaca(const KopijaBaca&) { throw std::runtime_error("kopija baca"); }
    KopijaBaca& operator=(const KopijaBaca&) = default;
};

void sekcija6() {
    std::cout << "\n== 6. variant kao mašina stanja\n";
    const char* imena[] = {"Ugasen", "Radi", "Greska"};
    Stanje s = Ugasen{};
    for (const char* d : {"start", "brze", "brze", "kvar", "brze", "reset"}) {
        s = sledece(s, d);
        std::cout << d << " -> " << imena[s.index()];
        if (auto* r = std::get_if<Radi>(&s)) std::cout << " (brzina " << r->brzina << ')';
        if (auto* g = std::get_if<Greska>(&s)) std::cout << " (" << g->opis << ')';
        std::cout << '\n';
    }

    std::variant<int, KopijaBaca> v = 5;
    KopijaBaca izvor;
    try {
        v = izvor;                                      // stari int uništen, kopija baci -- nema ni jednog ni drugog
    } catch (const std::exception&) {
    }
    std::cout << "valueless_by_exception: " << v.valueless_by_exception()
              << ", index == variant_npos: " << (v.index() == std::variant_npos) << '\n';
}

// ---------------------------------------------------------------- 7
struct Veliki {
    char bajtovi[64];
};

void sekcija7() {
    std::cout << "\n== 7. std::any\n";
    std::map<std::string, std::any> svojstva;
    svojstva["id"] = 7;
    svojstva["ime"] = std::string("temp");
    svojstva["aktivan"] = true;
    std::cout << "id " << std::any_cast<int>(svojstva["id"]) << ", ime " << std::any_cast<std::string>(svojstva["ime"])
              << '\n';
    std::cout << "type() == typeid(bool): " << (svojstva["aktivan"].type() == typeid(bool)) << '\n';
    if (auto* p = std::any_cast<double>(&svojstva["id"]))   // pokazivač: nullptr ako tip nije tačan
        std::cout << *p << '\n';
    else
        std::cout << "any_cast<double>(&a): nullptr\n";
    try {
        std::any_cast<double>(svojstva["id"]);
    } catch (const std::bad_any_cast&) {
        std::cout << "any_cast<double> na int: bad_any_cast\n";
    }
    std::any literal = "abc";                           // const char*, ne std::string (zadatak z3)
    std::cout << "\"abc\" u any: const char* " << (literal.type() == typeid(const char*)) << '\n';

    alokacija = 0;
    std::any mali = 5;
    int zaMali = alokacija;
    alokacija = 0;
    std::any veliki = Veliki{};
    std::cout << "alokacija: any sa int " << zaMali << ", any sa 64 B " << alokacija << '\n';
    std::any prazan;
    std::cout << "sizeof(any) " << sizeof(std::any) << ", prazan.has_value() " << prazan.has_value() << '\n';
    (void)mali;
    (void)veliki;
}

int main() {
    std::cout << std::boolalpha;
    sekcija1();
    sekcija2();
    sekcija3();
    sekcija4();
    sekcija5();
    sekcija6();
    sekcija7();
}
