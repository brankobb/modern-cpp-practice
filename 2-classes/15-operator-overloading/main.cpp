#include <algorithm>
#include <cstring>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// Operator overloading -- ISPRAVNI slučajevi. Sve se kompajlira i radi bez
// ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// C++20 poređenje (<=>, = default za ==) je u main_cpp20.cpp:
//   ./build.sh 2-classes/15-operator-overloading/main_cpp20.cpp -std=c++20
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh 2-classes/15-operator-overloading  proverava oba.

// ---------------------------------------------------------------- 1
class Money {
public:
    explicit Money(long cents) : cents_(cents) {}
    long cents() const { return cents_; }

    // Složena dodela je MEMBER: menja levi operand i vraća ga po referenci.
    Money& operator+=(const Money& rhs) {
        cents_ += rhs.cents_;
        return *this;
    }

private:
    long cents_;
};

// Binarni + je SLOBODNA funkcija napisana preko +=: ne menja operande,
// vraća novu vrednost. Tako logika postoji na jednom mestu.
Money operator+(Money lhs, const Money& rhs) { // lhs po vrednosti: to je već kopija koju vraćamo
    lhs += rhs;
    return lhs;
}

void s01_basics() {
    std::cout << "-- 1. an operator is a function --\n";
    Money a(150);
    Money b(275);
    Money sum1 = a + b;           // kompajler to prevede u:
    Money sum2 = operator+(a, b); // isti poziv, napisan kao funkcija
    a += b;                       // a.operator+=(b)
    std::cout << "  a + b = " << sum1.cents() << ", operator+(a, b) = " << sum2.cents()
              << ", after a += b: a = " << a.cents() << "\n";
}

// ---------------------------------------------------------------- 2
// EC++ Item 24: kad konverzija treba da radi za OBA operanda, operator je
// slobodna funkcija. Rational(int) namerno NIJE explicit: 2 je racionalan broj.
class Rational {
public:
    Rational(int numerator = 0, int denominator = 1) : num_(numerator), den_(denominator) { normalize(); }
    int num() const { return num_; }
    int den() const { return den_; }

    Rational& operator*=(const Rational& rhs) {
        num_ *= rhs.num_;
        den_ *= rhs.den_;
        normalize();
        return *this;
    }

private:
    void normalize() {
        if (den_ == 0) throw std::invalid_argument("denominator is 0");
        if (den_ < 0) {
            num_ = -num_;
            den_ = -den_;
        }
        int g = std::gcd(num_, den_);
        if (g > 1) {
            num_ /= g;
            den_ /= g;
        }
    }
    int num_;
    int den_;
};

Rational operator*(Rational lhs, const Rational& rhs) {
    lhs *= rhs;
    return lhs;
}

std::string str(const Rational& r) { return std::to_string(r.num()) + "/" + std::to_string(r.den()); }

void s02_memberVsFree() {
    std::cout << "-- 2. member vs free function (EC++ Item 24) --\n";
    Rational third(1, 3);
    Rational a = third * 2; // Rational(2) pa operator*(third, Rational(2))
    Rational b = 2 * third; // radi SAMO zato što je operator* slobodna funkcija
    std::cout << "  third * 2 = " << str(a) << ", 2 * third = " << str(b)
              << "  <- as a member, 2 * third would not compile (errors/e03)\n";
}

// ---------------------------------------------------------------- 3
class Temperature {
public:
    explicit Temperature(double celsius) : celsius_(celsius) {}

    // "Hidden friend": definisan UNUTAR klase, a nije član. Vidi private
    // podatke, a pronalazi ga samo ADL (lekcija 08), pa ne zagađuje
    // okolni namespace i ne učestvuje u tuđim overload-ima.
    friend std::ostream& operator<<(std::ostream& os, const Temperature& t) {
        return os << t.celsius_ << " C";
    }
    friend bool operator==(const Temperature& a, const Temperature& b) { return a.celsius_ == b.celsius_; }
    friend bool operator!=(const Temperature& a, const Temperature& b) { return !(a == b); } // preko ==

private:
    double celsius_;
};

void s03_friend() {
    std::cout << "-- 3. friend: a free function with access to the private part --\n";
    Temperature t(21.5);
    Temperature same(21.5);
    Temperature other(-3.0);
    std::cout << std::boolalpha << "  cout << t: " << t << ", t == same: " << (t == same)
              << ", t != other: " << (t != other) << "\n" << std::noboolalpha;
}

// ---------------------------------------------------------------- 4
class Name {
public:
    explicit Name(const char* text) : data_(new char[std::strlen(text) + 1]) { std::strcpy(data_, text); }
    Name(const Name& other) : Name(other.data_) {}
    ~Name() { delete[] data_; }

    // Copy dodela (EC++ Item 10 i 11): vraća *this po referenci da bi
    // a = b = c radilo, i bezbedna je za a = a. Copy-and-swap: prvo se
    // napravi kopija (ako new baci, *this je netaknut), pa se zameni sadržaj.
    // Naivna verzija "delete[] data_; pa kopiraj other.data_" pukne kad je
    // other isti objekat (ub/u01).
    Name& operator=(const Name& other) {
        Name copy(other);
        std::swap(data_, copy.data_);
        return *this; // copy odnosi staru memoriju u svom destruktoru
    }

    const char* c_str() const { return data_; }

private:
    char* data_;
};

void s04_assignment() {
    std::cout << "-- 4. operator= (assignment) --\n";
    Name a("Ann");
    Name b("Bob");
    Name c("Cindy");
    a = b = c; // desno asocijativno: a = (b = c), zato = vraća referencu
    std::cout << "  a = b = c: a=" << a.c_str() << " b=" << b.c_str() << " c=" << c.c_str() << "\n";
    Name& alias = a;
    a = alias; // dodela samom sebi: copy-and-swap je bezbedan
    std::cout << "  a = a: a=" << a.c_str() << "\n";
    // operator= mora biti član klase (errors/e05). Detaljno (rule of 3,
    // move dodela): lekcije 20–22.
}

// ---------------------------------------------------------------- 5
struct Version {
    int major;
    int minor;
};

bool operator==(const Version& a, const Version& b) { return a.major == b.major && a.minor == b.minor; }
bool operator!=(const Version& a, const Version& b) { return !(a == b); }
bool operator<(const Version& a, const Version& b) { // std::tie poredi redom, kao rečnik
    return std::tie(a.major, a.minor) < std::tie(b.major, b.minor);
}
bool operator>(const Version& a, const Version& b) { return b < a; } // ostali preko <
bool operator<=(const Version& a, const Version& b) { return !(b < a); }
bool operator>=(const Version& a, const Version& b) { return !(a < b); }

void s05_comparison() {
    std::cout << "-- 5. comparison (the C++17 way; C++20 in main_cpp20.cpp) --\n";
    std::vector<Version> versions{{2, 0}, {1, 10}, {1, 2}};
    std::sort(versions.begin(), versions.end()); // koristi operator<
    std::cout << "  sorted:";
    for (const Version& v : versions) std::cout << " " << v.major << "." << v.minor;
    std::cout << std::boolalpha << "\n  {1,2} < {1,10}: " << (Version{1, 2} < Version{1, 10})
              << ", {2,0} >= {1,10}: " << (Version{2, 0} >= Version{1, 10})
              << ", {1,2} != {1,2}: " << (Version{1, 2} != Version{1, 2}) << "\n" << std::noboolalpha;
}

// ---------------------------------------------------------------- 6
std::ostream& operator<<(std::ostream& os, const Rational& r) { return os << r.num() << "/" << r.den(); }

std::istream& operator>>(std::istream& is, Rational& r) {
    int n = 0;
    int d = 1;
    char slash = 0;
    if (is >> n >> slash >> d && slash == '/' && d != 0) {
        r = Rational(n, d);
    } else {
        is.setstate(std::ios::failbit); // loš ulaz: stream u fail stanje, r ostaje isti
    }
    return is;
}

void s06_streams() {
    std::cout << "-- 6. operator<< and operator>> for streams --\n";
    std::istringstream good("6/8");
    std::istringstream bad("6x8");
    Rational r1;
    Rational r2(5, 7);
    good >> r1;
    bad >> r2;
    std::cout << std::boolalpha << "  \"6/8\" -> " << r1 << " (ok=" << !good.fail() << ")"
              << ", \"6x8\" -> " << r2 << " (ok=" << !bad.fail() << ", value unchanged)\n"
              << std::noboolalpha;
}

// ---------------------------------------------------------------- 7
class Grid {
public:
    Grid(int rows, int cols) : rows_(rows), cols_(cols), cells_(static_cast<std::size_t>(rows * cols), 0) {}

    // Dve verzije: const vraća const referencu (čitanje), ne-const
    // referencu (čitanje i pisanje). Bez const verzije, const Grid& ne bi
    // mogao da čita (errors/e10). Kao kod std::vector: [] bez provere.
    int& operator[](int i) { return cells_[static_cast<std::size_t>(i)]; }
    const int& operator[](int i) const { return cells_[static_cast<std::size_t>(i)]; }

    // Sa proverom, kao std::vector::at.
    int& at(int r, int c) {
        if (r < 0 || r >= rows_ || c < 0 || c >= cols_) throw std::out_of_range("Grid::at");
        return cells_[static_cast<std::size_t>(r * cols_ + c)];
    }

private:
    int rows_;
    int cols_;
    std::vector<int> cells_;
};

int sumFirstTwo(const Grid& g) { return g[0] + g[1]; } // poziva const verziju

void s07_subscript() {
    std::cout << "-- 7. operator[] (const and non-const) --\n";
    Grid g(2, 3);
    g[0] = 4;       // ne-const verzija vraća int&, pa može dodela
    g[1] = 5;
    g.at(1, 2) = 9;
    std::cout << "  g[0]=" << g[0] << " sumFirstTwo(const Grid&)=" << sumFirstTwo(g) << " g.at(1, 2)=" << g.at(1, 2);
    try {
        g.at(5, 0) = 1;
    } catch (const std::out_of_range&) {
        std::cout << ", g.at(5, 0) -> std::out_of_range";
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------- 8
class Counter {
public:
    Counter& operator++() { // prefiks ++c: uveća i vrati SEBE
        ++value_;
        return *this;
    }
    Counter operator++(int) { // postfiks c++: int je samo oznaka; vraća STARU vrednost (kopija)
        Counter old = *this;
        ++*this;               // postfiks napisan preko prefiksa
        return old;
    }
    int value() const { return value_; }

private:
    int value_ = 0;
};

void s08_increment() {
    std::cout << "-- 8. ++ prefix and postfix --\n";
    Counter c;
    int pre = (++c).value();  // 1: vrednost posle uvećanja
    int post = (c++).value(); // 1: stara vrednost; c je sada 2
    std::cout << "  (++c).value()=" << pre << " (c++).value()=" << post << " c.value()=" << c.value()
              << "  <- postfix makes a copy, hence ++it in loops\n";
}

// ---------------------------------------------------------------- 9
class ByLength { // function object: klasa sa operator()
public:
    bool operator()(const std::string& a, const std::string& b) {
        ++calls_;
        return a.size() < b.size();
    }
    int calls() const { return calls_; }

private:
    int calls_ = 0; // za razliku od obične funkcije, može da ima stanje
};

void s09_callOperator() {
    std::cout << "-- 9. operator(): a function object --\n";
    std::vector<std::string> words{"lambda", "c", "abc", "if"};
    ByLength byLength;
    std::string a = "ab";
    std::string b = "abc";
    bool shorter = byLength(a, b); // poziv objekta kao funkcije: byLength.operator()(a, b)
    std::cout << std::boolalpha << "  byLength(\"ab\", \"abc\")=" << shorter << ", byLength.calls()=" << byLength.calls()
              << std::noboolalpha;
    std::sort(words.begin(), words.end(), ByLength{}); // std::sort prima i objekat, kao lambdu
    std::cout << ", sorted by length:";
    for (const std::string& w : words) std::cout << " " << w;
    std::cout << "\n  a lambda is exactly this: the compiler creates a class with operator()\n";
}

// ---------------------------------------------------------------- 10
struct Widget {
    int value = 7;
    int twice() const { return 2 * value; }
};

// Mali vlasnički pametni pokazivač: operator* i operator-> ga čine da se
// koristi kao pokazivač, a destruktor briše objekat. Pravi std::unique_ptr
// i sopstveni UniquePtr: lekcije 32 i 33.
template <typename T>
class ScopedPtr {
public:
    explicit ScopedPtr(T* p = nullptr) : p_(p) {}
    ~ScopedPtr() { delete p_; }
    ScopedPtr(const ScopedPtr&) = delete; // dva vlasnika = dva delete
    ScopedPtr& operator=(const ScopedPtr&) = delete;

    T& operator*() const { return *p_; }
    T* operator->() const { return p_; } // ptr->twice() postaje (ptr.operator->())->twice()
    explicit operator bool() const { return p_ != nullptr; } // if (ptr) -- konverzije: lekcija 17

private:
    T* p_;
};

void s10_smartPointerOperators() {
    std::cout << "-- 10. operator* and operator->: a small smart pointer --\n";
    ScopedPtr<Widget> ptr(new Widget);
    ptr->value = 20;      // operator->
    (*ptr).value += 1;    // operator*
    ScopedPtr<Widget> empty;
    std::cout << std::boolalpha << "  ptr->twice()=" << ptr->twice() << ", (*ptr).value=" << (*ptr).value
              << ", bool(ptr)=" << static_cast<bool>(ptr) << ", bool(empty)=" << static_cast<bool>(empty)
              << "  (empty-> would be UB, ub/u03)\n" << std::noboolalpha;
} // ptr briše Widget ovde

// ---------------------------------------------------------------- 11
struct Flag {
    bool on;
    const char* name;
};

int evaluated = 0;
Flag check(bool on, const char* name) {
    ++evaluated;
    return Flag{on, name};
}

// NE RADI OVAKO: preopterećen && gubi "short-circuit". Oba operanda se
// izračunaju pre poziva, jer je to obična funkcija sa dva argumenta.
bool operator&&(const Flag& a, const Flag& b) { return a.on && b.on; }

void s11_rules() {
    std::cout << "-- 11. rules: do not overload &&, || and , --\n";
    evaluated = 0;
    bool builtin = false && check(true, "right").on; // ugrađeni &&: desna strana se ne računa
    int afterBuiltin = evaluated;
    evaluated = 0;
    bool custom = check(false, "left") && check(true, "right"); // naš &&: računaju se obe
    std::cout << std::boolalpha << "  built-in false && x: " << builtin << ", evaluated=" << afterBuiltin
              << "; Flag && Flag: " << custom << ", evaluated=" << evaluated << "\n" << std::noboolalpha;
}

int main() {
    s01_basics();
    s02_memberVsFree();
    s03_friend();
    s04_assignment();
    s05_comparison();
    s06_streams();
    s07_subscript();
    s08_increment();
    s09_callOperator();
    s10_smartPointerOperators();
    s11_rules();
}
