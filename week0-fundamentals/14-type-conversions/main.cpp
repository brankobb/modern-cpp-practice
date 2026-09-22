#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>
#if __cplusplus >= 202002L
#include <bit>
#endif

// Konverzije tipova -- ISPRAVNI slučajevi. Sve se kompajlira i radi bez
// ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh week0-fundamentals/14-type-conversions  proverava oba.

// ---------------------------------------------------------------- 1
void s01_implicitConversions() {
    std::cout << "-- 1. implicitne (standardne) konverzije --\n";
    char a = 'a';
    char b = 'b';
    auto sum = a + b; // char + char -> int (integralna promocija, lekcija 01)
    std::cout << "  'a' + 'b' je tipa int: " << (sizeof(sum) == sizeof(int) ? "da" : "ne") << ", vrednost " << sum << "\n";

    int i = 7;
    double d = i;                     // int -> double: bez gubitka
    double pi = 3.99;
    int truncated = static_cast<int>(pi);    // double -> int: odseca prema nuli
    int negative = static_cast<int>(-pi);
    std::cout << "  int 7 -> double " << d << "; static_cast<int>(3.99)=" << truncated
              << " static_cast<int>(-3.99)=" << negative << "  <- odseca, ne zaokružuje\n";

    unsigned int wrapped = static_cast<unsigned int>(-1); // definisano: modulo 2^32
    std::cout << "  static_cast<unsigned>(-1)=" << wrapped << "  <- zato je -1 < 1u netačno (-Wsign-compare)\n";

    int* ptr = &i;
    int* none = nullptr;
    bool hasPtr = ptr;   // pokazivač -> bool: nullptr je false
    bool hasNone = none;
    std::cout << std::boolalpha << "  bool(ptr)=" << hasPtr << " bool(nullptr)=" << hasNone << "\n" << std::noboolalpha;
}

// ---------------------------------------------------------------- 2
enum class Color { Red = 1, Green = 2, Blue = 4 };

struct Base {
    virtual ~Base() = default;
    virtual std::string name() const { return "Base"; }
};
struct Derived : Base {
    std::string name() const override { return "Derived"; }
    int extra = 42;
};

void s02_staticCast() {
    std::cout << "-- 2. static_cast: proverene konverzije pri kompajliranju --\n";
    int total = 7;
    int count = 2;
    double avg = static_cast<double>(total) / count; // bez cast-a: 7 / 2 = 3
    int green = static_cast<int>(Color::Green);       // enum class -> int (nema implicitne)
    Color blue = static_cast<Color>(4);               // int -> enum class
    std::cout << "  7 / 2 = " << total / count << ", static_cast<double>(7) / 2 = " << avg
              << "; int(Color::Green)=" << green << ", Color(4)==Blue: " << (blue == Color::Blue ? "da" : "ne") << "\n";

    int value = 5;
    void* raw = &value;
    int* back = static_cast<int*>(raw); // void* -> T*: samo nazad u PRAVI tip
    std::cout << "  void* -> int*: *back=" << *back << "\n";

    Derived derived;
    Base* up = &derived;                          // izvedena -> bazna: implicitno
    Derived* down = static_cast<Derived*>(up);    // bazna -> izvedena: BEZ provere (13, ub/u03)
    std::cout << "  upcast implicitno, static_cast naniže (kad ZNAŠ tip): down->extra=" << down->extra << "\n";
}

// ---------------------------------------------------------------- 3
void s03_reinterpretCast() {
    std::cout << "-- 3. reinterpret_cast i bajtovi objekta --\n";
    std::uint32_t word = 0x11223344;
    // unsigned char* (i std::byte*) sme da čita bajtove BILO KOG objekta.
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&word);
    std::cout << "  prvi bajt od 0x11223344: 0x" << std::hex << static_cast<int>(bytes[0]) << std::dec
              << (bytes[0] == 0x44 ? " (little-endian)" : " (big-endian)") << "\n";

    int value = 9;
    std::uintptr_t address = reinterpret_cast<std::uintptr_t>(&value); // pokazivač -> broj
    int* again = reinterpret_cast<int*>(address);                      // i nazad: isti pokazivač
    std::cout << "  int* -> uintptr_t -> int*: *again=" << *again << "\n";

    // Bitovi float-a kao int: NE reinterpret_cast<int*>(&f) (strict aliasing,
    // vidi notes), nego memcpy ili C++20 std::bit_cast.
    float f = 1.0f;
    std::uint32_t viaMemcpy = 0;
    std::memcpy(&viaMemcpy, &f, sizeof f);
#if __cplusplus >= 202002L
    std::uint32_t viaBitCast = std::bit_cast<std::uint32_t>(f);
#else
    std::uint32_t viaBitCast = viaMemcpy; // std::bit_cast postoji tek od C++20
#endif
    std::cout << "  bitovi 1.0f: memcpy=0x" << std::hex << viaMemcpy << " bit_cast=0x" << viaBitCast << std::dec << "\n";
}

// ---------------------------------------------------------------- 4
struct Animal {
    virtual ~Animal() = default; // dynamic_cast radi samo za polimorfne tipove (errors/e03)
};
struct Dog : Animal {
    std::string bark() const { return "Av"; }
};
struct Cat : Animal {};
struct Pet {
    virtual ~Pet() = default;
};
struct HouseDog : Dog, Pet {};

std::string describe(Animal& a) {
    if (Dog* dog = dynamic_cast<Dog*>(&a)) return "Dog: " + dog->bark(); // proveri rezultat! (ub/u01)
    return "nije Dog";
}

void s04_dynamicCast() {
    std::cout << "-- 4. dynamic_cast: provera tipa pri izvršavanju --\n";
    Dog dog;
    Cat cat;
    std::cout << "  pokazivač: describe(dog)=" << describe(dog) << ", describe(cat)=" << describe(cat)
              << " (nullptr umesto UB)\n";
    try {
        Animal& a = cat;
        Dog& d = dynamic_cast<Dog&>(a); // referenca ne može biti null -> izuzetak
        std::cout << d.bark();
    } catch (const std::bad_cast&) {
        std::cout << "  referenca: dynamic_cast<Dog&>(cat) -> std::bad_cast\n";
    }
    HouseDog house;
    Animal& asAnimal = house;
    Pet* asPet = dynamic_cast<Pet*>(&asAnimal); // "cross-cast": iz jedne grane hijerarhije u drugu
    std::cout << "  cross-cast Animal& -> Pet*: " << (asPet != nullptr ? "uspeo" : "nullptr") << "\n";
}

// ---------------------------------------------------------------- 5
class Meters {
public:
    explicit Meters(double value) : value_(value) {} // explicit: nema tihe konverzije iz double
    double value() const { return value_; }

private:
    double value_;
};

class Percent {
public:
    Percent(int value) : value_(value) {} // NAMERNO bez explicit: 50 JESTE procenat (retko je ovako)
    int value() const { return value_; }

private:
    int value_;
};

double twice(Meters m) { return 2 * m.value(); }
int half(Percent p) { return p.value() / 2; }

void s05_convertingConstructor() {
    std::cout << "-- 5. konstruktor kao konverzija (primitivni -> korisnički tip) --\n";
    std::cout << "  twice(Meters(5.0))=" << twice(Meters(5.0)) << "  (twice(5.0) se ne kompajlira, errors/e04)\n";
    std::cout << "  half(50)=" << half(50) << "  <- 50 -> Percent(50) tiho, jer konstruktor nije explicit\n";
}

// ---------------------------------------------------------------- 6
class Fraction {
public:
    Fraction(int num, int den) : num_(num), den_(den) {}
    explicit operator double() const { return static_cast<double>(num_) / den_; } // korisnički -> primitivni

private:
    int num_;
    int den_;
};

class Connection {
public:
    explicit Connection(bool open) : open_(open) {}
    // explicit operator bool: radi u if, !, &&, ||, ?: ("kontekstualna"
    // konverzija), ali ne i u bool b = c; ili c + 1 (errors/e05).
    explicit operator bool() const { return open_; }

private:
    bool open_;
};

void s06_conversionOperator() {
    std::cout << "-- 6. operator konverzije (korisnički -> primitivni tip) --\n";
    Fraction threeQuarters(3, 4);
    double asDouble = static_cast<double>(threeQuarters); // explicit: mora cast
    std::cout << "  static_cast<double>(Fraction(3, 4))=" << asDouble << "\n";

    Connection open(true);
    Connection closed(false);
    std::cout << "  if (open): " << (open ? "otvorena" : "zatvorena") << ", !closed: " << (!closed ? "true" : "false")
              << ", open && !closed: " << ((open && !closed) ? "true" : "false") << "\n";
}

// ---------------------------------------------------------------- 7
struct Fahrenheit {
    double degrees;
};

class Celsius {
public:
    explicit Celsius(double degrees) : degrees_(degrees) {}
    // Konverzija iz DRUGE korisničke klase: konstruktor u odredišnoj klasi.
    // Ne pisati i Fahrenheit::operator Celsius() -- dva puta za istu
    // konverziju je dvosmisleno (errors/e06).
    explicit Celsius(const Fahrenheit& f) : degrees_((f.degrees - 32.0) * 5.0 / 9.0) {}
    double degrees() const { return degrees_; }

private:
    double degrees_;
};

void s07_userToUser() {
    std::cout << "-- 7. korisnički -> korisnički tip --\n";
    Fahrenheit boiling{212.0};
    Celsius c(boiling);
    std::cout << "  Celsius(Fahrenheit{212})=" << c.degrees() << "\n";
}

// ---------------------------------------------------------------- 8
// Proverena konverzija broja: baca ako se vrednost promeni (kao gsl::narrow).
template <typename To, typename From>
To narrow(From value) {
    To result = static_cast<To>(value);
    if (static_cast<From>(result) != value || ((result < To{}) != (value < From{}))) {
        throw std::range_error("narrow: vrednost ne staje u ciljni tip");
    }
    return result;
}

void s08_checkedNarrowing() {
    std::cout << "-- 8. proverena konverzija brojeva --\n";
    std::cout << "  numeric_limits<short>: [" << std::numeric_limits<short>::min() << ", "
              << std::numeric_limits<short>::max() << "]\n";
    std::cout << "  narrow<short>(1000)=" << narrow<short>(1000);
    try {
        narrow<short>(100000);
    } catch (const std::range_error&) {
        std::cout << ", narrow<short>(100000) -> std::range_error";
    }
    try {
        narrow<unsigned>(-1);
    } catch (const std::range_error&) {
        std::cout << ", narrow<unsigned>(-1) -> std::range_error";
    }
    std::cout << "\n  (static_cast<short>(100000) bi tiho dao " << static_cast<short>(100000) << ")\n";
}

int main() {
    s01_implicitConversions();
    s02_staticCast();
    s03_reinterpretCast();
    s04_dynamicCast();
    s05_convertingConstructor();
    s06_conversionOperator();
    s07_userToUser();
    s08_checkedNarrowing();
}
