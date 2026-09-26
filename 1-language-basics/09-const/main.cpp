#include <array>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// const -- ISPRAVNI slučajevi. Sve u ovom fajlu se kompajlira i radi bez
// ASan/UBSan prijava (g++ 13 i clang 18).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 1-language-basics/09-const  proverava oba.
// Numeracija sekcija prati notes.md.

// ---------------------------------------------------------------- 1
struct Plain {
    int x;
};

int readSensor() { return 42; } // "runtime" vrednost

void s01_basics() {
    std::cout << "-- 1. const basics --\n";
    const int fromRuntime = readSensor(); // const NE znači "poznato pri kompajliranju"
    const std::string name;               // OK: string ima korisnički default ctor -> prazan
    const Plain p{};                      // OK: {} -> x == 0 (bez {} greška -- errors/e14)
    std::cout << "fromRuntime=" << fromRuntime << " name.size()=" << name.size()
              << " p.x=" << p.x << "\n";
    // const int x;  -> greška: mora odmah da dobije vrednost (errors/e01)
    // fromRuntime = 1; -> greška (errors/e02)
}

// ---------------------------------------------------------------- 2
void s02_pointers() {
    std::cout << "-- 2. const and pointers --\n";
    // "west const" i "east const" su ISTI tip -- const se odnosi na ono levo
    // od sebe, a ako levo nema ništa, na ono desno.
    static_assert(std::is_same_v<const int*, int const*>);
    static_assert(!std::is_same_v<const int*, int* const>);

    int x = 1;
    int* p = &x;
    int** pp = &p;
    const int* cp = p;            // int* -> const int*: dodavanje const je OK
    const int* const* cpp = pp;   // int** -> const int* const*: OK
    // const int** bad = pp;      -> greška (errors/e03), vidi notes.md
    std::cout << "*cp=" << *cp << " **cpp=" << **cpp << "\n";
}

// ---------------------------------------------------------------- 3
std::size_t countChars(const std::string& s) { return s.size(); } // čita, ne kopira

void s03_constReference() {
    std::cout << "-- 3. const reference --\n";
    int x = 1;
    const int& r = x; // kroz r samo čitanje (errors/e17); x se i dalje menja direktno
    x = 2;
    std::cout << "r after x = 2: " << r << "\n";
    std::cout << "countChars(\"literal\") = " << countChars("literal")
              << " (const& also accepts a temporary std::string)\n";
}

// ---------------------------------------------------------------- 4
class TextBlock {
public:
    explicit TextBlock(std::string text) : text_(std::move(text)) {}

    // EC++ Item 3: const i non-const overload. Kompajler bira po tome da li
    // je OBJEKAT const.
    const char& operator[](std::size_t i) const {
        std::cout << "  [const operator[]]\n";
        return text_[i];
    }
    // Non-const verzija bez dupliranja koda: pozovi const verziju, pa skini
    // const sa rezultata. Bezbedno je jer je *this ovde sigurno ne-const.
    char& operator[](std::size_t i) {
        std::cout << "  [non-const operator[] -> ";
        return const_cast<char&>(static_cast<const TextBlock&>(*this)[i]);
    }
    const std::string& text() const { return text_; }

private:
    std::string text_;
};

class Buffer {
public:
    explicit Buffer(char* data) : data_(data) {}
    // "Bitwise const": funkcija ne menja NIJEDAN član (pokazivač data_ ostaje
    // isti), pa se kompajlira -- ali menja ono na šta pokazuje. Kompajler
    // proverava samo bitove objekta; LOGIČKU konstantnost čuvaš ti.
    void scribble() const { data_[0] = '#'; }

private:
    char* data_;
};

void s04_constMemberFunctions() {
    std::cout << "-- 4. const member functions (EC++ Item 3) --\n";
    TextBlock tb("Hello");
    const TextBlock ctb("World");
    std::cout << "tb[0] (non-const object):\n";
    tb[0] = 'J';
    std::cout << "ctb[0] (const object):\n";
    char c = ctb[0];   // ctb[0] = 'X'; -> greška: vraća const char&
    std::cout << "tb.text()=" << tb.text() << " ctb[0]=" << c << "\n";

    char raw[] = "abc";
    const Buffer buf(raw);
    buf.scribble();
    std::cout << "after const Buffer::scribble(): " << raw << " (bitwise const, not logical)\n";
}

// ---------------------------------------------------------------- 5
class Polynomial {
public:
    explicit Polynomial(double a) : a_(a) {}

    // Spolja je ovo čisto čitanje -> const. Keš je interni detalj -> mutable.
    // EMC Item 16: const funkcije se smeju zvati iz više niti istovremeno,
    // pa mutable stanje mora da bude zaštićeno (mutex ili atomic).
    double expensiveValue() const {
        ++calls_; // atomic -- bezbedno bez lock-a
        std::lock_guard<std::mutex> lock(mutex_);
        if (!cacheValid_) {
            cachedValue_ = a_ * a_; // "skupo" računanje
            cacheValid_ = true;
        }
        return cachedValue_;
    }
    int calls() const { return calls_.load(); }

private:
    double a_;
    mutable std::mutex mutex_;
    mutable bool cacheValid_ = false;
    mutable double cachedValue_ = 0.0;
    mutable std::atomic<int> calls_{0};
};

void s05_mutable() {
    std::cout << "-- 5. mutable i thread-safety (EMC Item 16) --\n";
    const Polynomial p(3.0);
    double v1 = p.expensiveValue();
    double v2 = p.expensiveValue();
    std::cout << "value=" << v1 << "," << v2 << " call count=" << p.calls() << "\n";
}

// ---------------------------------------------------------------- 6
struct Tracer {
    Tracer() = default;
    Tracer(const Tracer&) = default;
    Tracer(Tracer&&) = default;
    Tracer& operator=(const Tracer&) {
        std::cout << "  copy assignment\n";
        return *this;
    }
    Tracer& operator=(Tracer&&) {
        std::cout << "  move assignment\n";
        return *this;
    }
};

const Tracer makeConst() { return Tracer{}; }
Tracer makePlain() { return Tracer{}; }

struct Rational {
    int n = 0;
    Rational() = default;
    Rational(int v) : n(v) {}
    Rational(const Rational&) = default;
    Rational(Rational&&) = default;
    // Dodela samo u lvalue (&) -- zabranjuje (a * b) = c bez const povratne vrednosti.
    Rational& operator=(const Rational&) & = default;
    Rational& operator=(Rational&&) & = default;
};
Rational operator*(const Rational& a, const Rational& b) { return Rational{a.n * b.n}; }

void s06_constReturn() {
    std::cout << "-- 6. const return value --\n";
    Tracer t;
    std::cout << "t = makeConst();  (const Tracer -> move is not possible):\n";
    t = makeConst();
    std::cout << "t = makePlain();  (plain value -> move):\n";
    t = makePlain();

    Rational a{2}, b{3}, c{4};
    a = b * c; // OK: dodela u lvalue, i to move
    std::cout << "a = b * c -> " << a.n << "  ((a * b) = c is an error because of & on operator=)\n";
}

// ---------------------------------------------------------------- 7
void legacyLog(char* msg) { std::cout << "  legacyLog: " << msg << "\n"; } // stari C API: ne menja msg

void s07_constCast() {
    std::cout << "-- 7. const_cast: when it is legitimate --\n";
    // 1) Stari API koji ne menja podatke, ali je deklarisan bez const.
    const std::string message = "message";
    legacyLog(const_cast<char*>(message.c_str()));

    // 2) Objekat SAM nije const -- samo pristup je bio kroz const pokazivač.
    int x = 1;
    const int* cp = &x;
    *const_cast<int*>(cp) = 2;  // definisano ponašanje: x nije const
    std::cout << "x after writing through const_cast = " << x << "\n";
    // Upis u objekat koji JESTE definisan kao const je UB (ub/u01, u02).
    // static_cast ne sme da skine const (errors/e16).
}

// ---------------------------------------------------------------- 8
void s08_stl() {
    std::cout << "-- 8. const and the STL (EMC Item 13) --\n";
    std::vector<int> v{3, 1, 2};
    int sum = 0;
    for (auto it = v.cbegin(); it != v.cend(); ++it) sum += *it; // samo čitanje
    std::cout << "sum via cbegin/cend = " << sum << "\n";

    const std::map<std::string, int> ages{{"Ann", 30}};
    // ages["Ann"] ne radi na const mapi (errors/e08):
    std::cout << "ages.at(\"Ann\") = " << ages.at("Ann") << "\n";
    auto found = ages.find("John");
    std::cout << "ages.find(\"John\") == end: " << std::boolalpha << (found == ages.end())
              << std::noboolalpha << "\n";

    // std::as_const: nateraj izbor const overload-a na ne-const objektu.
    TextBlock tb("xyz");
    std::cout << "std::as_const(tb)[0]:\n";
    char first = std::as_const(tb)[0];
    std::cout << "first=" << first << "\n";

    for (const auto& value : v) sum += value; // podrazumevani oblik za čitanje
    std::cout << "sum after range-for with const auto& = " << sum << "\n";
}

// ---------------------------------------------------------------- 9
constexpr int square(int n) { return n * n; }

void s09_constexpr() {
    std::cout << "-- 9. const vs constexpr (EMC Item 15) --\n";
    constexpr int n = square(4);   // izračunato pri kompajliranju
    std::array<int, n> a{};        // zato sme kao veličina
    const int m = 10;              // const int sa KONSTANTNIM inicijalizatorom
    int legacy[m] = {};            // ...je upotrebljiv u konstantnom izrazu
    static_assert(n == 16 && m == 10);
    constexpr double pi = 3.14159; // za double MORA constexpr (const double ne važi -- errors/e12)
    static_assert(pi > 3.0);
    int runtime = square(readSensor()); // constexpr funkcija radi i u runtime-u
    std::cout << "a.size()=" << a.size() << " sizeof(legacy)/sizeof(int)=" << sizeof(legacy) / sizeof(int)
              << " square(42)=" << runtime << "\n";
}

// ---------------------------------------------------------------- 10
template <typename T>
void deduce(T) {
    static_assert(std::is_same_v<T, int>); // top-level const odbačen i ovde
}

void s10_topLevelVsLowLevel() {
    std::cout << "-- 10. top-level vs low-level const --\n";
    const int ci = 1;
    const int* pci = &ci;
    const int* const cpci = &ci;

    auto a = ci;    // top-level const se odbacuje: kopija sme da se menja
    auto b = pci;   // low-level const (na pokazivanom) OSTAJE
    auto c = cpci;  // top-level odbačen, low-level ostaje
    decltype(ci) d = 2; // decltype čuva const
    static_assert(std::is_same_v<decltype(a), int>);
    static_assert(std::is_same_v<decltype(b), const int*>);
    static_assert(std::is_same_v<decltype(c), const int*>);
    static_assert(std::is_same_v<decltype(d), const int>);
    deduce(ci);
    a = 5;
    std::cout << "auto from const int -> int (modifiable: a=" << a
              << "), auto from const int* -> const int* (static_assert)\n";
    (void)b; (void)c; (void)d;
}

// ---------------------------------------------------------------- 11
void s11_lambda() {
    std::cout << "-- 11. lambdas: operator() is const by default --\n";
    int x = 0;
    auto next = [x]() mutable { return ++x; }; // menja SVOJU kopiju (errors/e18 bez mutable)
    int first = next();
    int second = next();
    std::cout << "next()=" << first << "," << second << " outer x=" << x << "\n";
}

int main() {
    s01_basics();
    s02_pointers();
    s03_constReference();
    s04_constMemberFunctions();
    s05_mutable();
    s06_constReturn();
    s07_constCast();
    s08_stl();
    s09_constexpr();
    s10_topLevelVsLowLevel();
    s11_lambda();
}
