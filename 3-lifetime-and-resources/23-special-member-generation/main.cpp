#include <iostream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Lekcija 23 -- pravila generisanja specijalnih funkcija: ISPRAVNI slučajevi.
// Sve se kompajlira i radi bez ASan/UBSan prijava (g++ 13 i clang 18,
// C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 3-lifetime-and-resources/23-special-member-generation  proverava oba.

// Član koji javlja da li je kopiran ili pomeren -- tako se vidi šta klasa
// koja ga sadrži STVARNO radi, ne samo šta type traits kažu.
struct Probe {
    Probe() = default;
    Probe(const Probe&) { std::cout << "copy"; }
    Probe(Probe&&) noexcept { std::cout << "move"; }
    Probe& operator=(const Probe&) {
        std::cout << "copy=";
        return *this;
    }
    Probe& operator=(Probe&&) noexcept {
        std::cout << "move=";
        return *this;
    }
};

// ---------------------------------------------------------------- 1
struct Nothing { Probe p; };                               // ništa deklarisano
struct DtorOnly { Probe p; ~DtorOnly() {} };               // samo destruktor
struct CopyCtorOnly {                                      // samo copy konstruktor
    Probe p;
    CopyCtorOnly() = default;
    CopyCtorOnly(const CopyCtorOnly& o) : p(o.p) {}
};
struct MoveCtorOnly {                                      // samo move konstruktor
    Probe p;
    MoveCtorOnly() = default;
    MoveCtorOnly(MoveCtorOnly&& o) noexcept : p(std::move(o.p)) {}
};
struct MoveAssignOnly {                                    // samo move dodela
    Probe p;
    MoveAssignOnly& operator=(MoveAssignOnly&& o) noexcept {
        p = std::move(o.p);
        return *this;
    }
};

template <typename T>
void row(const char* declared) {
    std::cout << "  " << declared << " copy ctor=" << std::is_copy_constructible_v<T>
              << " copy==" << std::is_copy_assignable_v<T> << " move ctor=" << std::is_move_constructible_v<T>
              << " move==" << std::is_move_assignable_v<T>;
    if constexpr (std::is_move_constructible_v<T>) {
        std::cout << " | T b = std::move(a) -> ";
        T a;
        T b = std::move(a); // šta se STVARNO pozove
        (void)b;
    }
    std::cout << "\n";
}

void s01_generationTable() {
    std::cout << "-- 1. what the compiler writes, depending on what you declared --\n";
    row<Nothing>("nothing:        ");
    row<DtorOnly>("destructor:     ");
    row<CopyCtorOnly>("copy ctor:      ");
    row<MoveCtorOnly>("move ctor:      ");
    row<MoveAssignOnly>("move assign:    ");
    std::cout << "  <- \"move ctor=1\" for destructor and copy ctor only means that T b = std::move(a) WORKS -- by copying!\n";
}

// ---------------------------------------------------------------- 2
// Deklarisan destruktor (npr. samo da bi bio virtual ili radi logovanja)
// ukida move. Rešenje: eksplicitno vrati sve (= default).
struct Logged {
    std::vector<int> data;
    ~Logged() {} // samo ovo -> move se NE generiše, "move" kopira ceo vector
};

struct LoggedFixed {
    std::vector<int> data;
    explicit LoggedFixed(std::vector<int> d) : data(std::move(d)) {} // C++20: klasa sa deklarisanim ctor-om nije agregat (lekcija 03)
    ~LoggedFixed() {}
    LoggedFixed(const LoggedFixed&) = default;
    LoggedFixed(LoggedFixed&&) = default;            // vraćeno
    LoggedFixed& operator=(const LoggedFixed&) = default;
    LoggedFixed& operator=(LoggedFixed&&) = default;
};

void s02_destructorKillsMove() {
    std::cout << "-- 2. a destructor suppresses move --\n";
    Logged a{std::vector<int>(1000, 1)};
    const int* before = a.data.data();
    Logged b = std::move(a);
    LoggedFixed c{std::vector<int>(1000, 1)};
    const int* beforeFixed = c.data.data();
    LoggedFixed d = std::move(c);
    std::cout << "  Logged (only ~):       same buffer after \"move\": " << (b.data.data() == before ? "yes" : "no")
              << ", the source still has " << a.data.size() << " elements  <- a copy!\n";
    std::cout << "  LoggedFixed (= default): same buffer: " << (d.data.data() == beforeFixed ? "yes" : "no")
              << ", the source has " << c.data.size() << " elements\n";
}

// ---------------------------------------------------------------- 3
struct HandWritten {
    std::string s;
    HandWritten() = default;
    HandWritten(HandWritten&& o) : s(std::move(o.s)) {} // ručno, zaboravljen noexcept
};
struct Defaulted {
    std::string s;
    Defaulted() = default;
    Defaulted(Defaulted&&) = default; // noexcept se izvede iz članova (string move je noexcept)
};

void s03_defaultAndNoexcept() {
    std::cout << "-- 3. = default deduces noexcept, a hand-written function does not --\n";
    std::cout << std::boolalpha << "  is_nothrow_move_constructible: HandWritten=" << std::is_nothrow_move_constructible_v<HandWritten>
              << " Defaulted=" << std::is_nothrow_move_constructible_v<Defaulted> << "\n" << std::noboolalpha;
}

// ---------------------------------------------------------------- 4
struct Element {
    Element() = default;
    Element(const Element&) { ++copies; }
    Element(Element&&) noexcept(false) { ++moves; } // move koji "može da baci"
    inline static int copies = 0;
    inline static int moves = 0;
};
struct SafeElement {
    SafeElement() = default;
    SafeElement(const SafeElement&) { ++copies; }
    SafeElement(SafeElement&&) noexcept { ++moves; }
    inline static int copies = 0;
    inline static int moves = 0;
};

void s04_vectorAndNoexcept() {
    std::cout << "-- 4. std::vector reallocation uses move only if it is noexcept --\n";
    std::vector<Element> risky(4);
    std::vector<SafeElement> safe(4);
    Element::copies = Element::moves = 0;
    SafeElement::copies = SafeElement::moves = 0;
    risky.reserve(100); // realokacija: premesti 4 elementa
    safe.reserve(100);
    std::cout << "  reserve(100) with 4 elements: Element (move noexcept(false)) copies=" << Element::copies
              << " move=" << Element::moves << "; SafeElement (noexcept) copies=" << SafeElement::copies
              << " move=" << SafeElement::moves << "\n";
    Element e;
    Element::copies = Element::moves = 0;
    Element taken = std::move_if_noexcept(e); // isto pravilo koje koristi vector
    (void)taken;
    std::cout << "  std::move_if_noexcept(Element) -> copies=" << Element::copies << " move=" << Element::moves << "\n";
}

// ---------------------------------------------------------------- 5
struct Box {
    Box() = default;
    Box(const Box&) { std::cout << "copy ctor"; }
    // Šablonski konstruktor NIJE copy konstruktor i ne sprečava da ga
    // kompajler napiše, ali u overload resolution-u često pobedi (EMC 26).
    template <typename T>
    explicit Box(T&&) { std::cout << "template ctor"; }
};

void s05_templateConstructor() {
    std::cout << "-- 5. a template constructor hijacks the copy (EMC Item 17 and 26) --\n";
    const Box constBox;
    Box plainBox;
    std::cout << "  Box a(constBox) -> ";
    Box a(constBox); // const Box& -> copy ctor je tačan match
    std::cout << "\n  Box b(plainBox) -> ";
    Box b(plainBox); // Box& -> šablon sa T = Box& je BOLJI (bez dodavanja const)
    std::cout << "  <- surprise (errors/e04)\n";
}

// ---------------------------------------------------------------- 6
struct Session {
    std::string user;
    Session() = default;
    Session(const Session&) = delete;            // kopija zabranjena
    Session& operator=(const Session&) = delete;
    Session(Session&&) = default;                // move dozvoljen
    Session& operator=(Session&&) = default;
};

Session login(const char* name) {
    Session s;
    s.user = name;
    return s; // move (ili NRVO); da je move obrisan -- greška (errors/e02)
}

void s06_moveOnly() {
    std::cout << "-- 6. = delete + = default: a move-only type --\n";
    Session s = login("ann");
    std::vector<Session> active;
    active.push_back(std::move(s));
    std::cout << std::boolalpha << "  Session: copy=" << std::is_copy_constructible_v<Session>
              << " move=" << std::is_move_constructible_v<Session> << ", active[0].user=" << active[0].user << "\n"
              << std::noboolalpha;
}

int main() {
    s01_generationTable();
    s02_destructorKillsMove();
    s03_defaultAndNoexcept();
    s04_vectorAndNoexcept();
    s05_templateConstructor();
    s06_moveOnly();
}
