#include <algorithm>
#include <cctype>
#include <cmath>
#include <compare>
#include <iostream>
#include <string>
#include <vector>

// C++20 poređenje: operator<=> (spaceship), "= default" za poređenje i
// prepisivanje izraza (rewritten candidates). Samo C++20:
//   ./build.sh 2-classes/15-operator-overloading/main_cpp20.cpp -std=c++20
// C++17 način (šest ručno napisanih operatora) je u main.cpp, sekcija 5.

const char* name(std::strong_ordering o) {
    return o < 0 ? "less" : o > 0 ? "greater" : "equal";
}

// ---------------------------------------------------------------- 1
struct Version {
    int major;
    int minor;
    // Jedna linija umesto šest operatora. Poredi članove redom deklaracije,
    // kao std::tie u main.cpp. Defaulted <=> implicitno daje i defaulted ==.
    auto operator<=>(const Version&) const = default;
};

void s01_defaultedSpaceship() {
    std::cout << "-- 1. auto operator<=>(const T&) const = default --\n";
    std::vector<Version> versions{{2, 0}, {1, 10}, {1, 2}};
    std::sort(versions.begin(), versions.end());
    std::cout << "  sortirano:";
    for (const Version& v : versions) std::cout << " " << v.major << "." << v.minor;
    Version a{1, 2};
    Version b{1, 10};
    std::cout << std::boolalpha << "\n  a < b: " << (a < b) << ", a >= b: " << (a >= b) << ", a == a: " << (a == a)
              << ", a != b: " << (a != b) << ", a <=> b: " << name(a <=> b) << "\n" << std::noboolalpha;
}

// ---------------------------------------------------------------- 2
class Username {
public:
    explicit Username(std::string s) : value_(std::move(s)) {}

    // Sopstveni <=>: poređenje bez obzira na velika/mala slova. "Ana" i
    // "ANA" su EKVIVALENTNI, ali nisu isti string -> weak_ordering.
    std::weak_ordering operator<=>(const Username& other) const {
        std::string a = lower(value_);
        std::string b = lower(other.value_);
        return a <=> b; // strong_ordering se pretvara u weak_ordering
    }
    // Ručno napisan <=> NE daje ==. Ako ga ne napišeš, a == b se ne
    // kompajlira (errors/e08). == je poseban jer često može brže.
    bool operator==(const Username& other) const { return (*this <=> other) == 0; }

private:
    static std::string lower(std::string s) {
        for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return s;
    }
    std::string value_;
};

void s02_customSpaceship() {
    std::cout << "-- 2. sopstveni <=> i poseban == --\n";
    Username a("Ana");
    Username upper("ANA");
    Username b("bojan");
    std::cout << std::boolalpha << "  Ana == ANA: " << (a == upper) << ", Ana < bojan: " << (a < b)
              << ", ANA > bojan: " << (upper > b) << "\n" << std::noboolalpha;
}

// ---------------------------------------------------------------- 3
struct Meters {
    int value;
    bool operator==(int other) const { return value == other; } // samo JEDAN operator
};

void s03_rewrittenCandidates() {
    std::cout << "-- 3. prepisani izrazi: != i obrnuti redosled dolaze sami --\n";
    Meters m{5};
    // C++17: za svaki od ovih trebao je poseban operator (m == 5, 5 == m,
    // m != 5, 5 != m). C++20 prepiše: 5 == m -> m == 5, m != 5 -> !(m == 5).
    std::cout << std::boolalpha << "  m == 5: " << (m == 5) << ", 5 == m: " << (5 == m) << ", m != 7: " << (m != 7)
              << ", 7 != m: " << (7 != m) << "\n" << std::noboolalpha;
}

// ---------------------------------------------------------------- 4
void s04_partialOrdering() {
    std::cout << "-- 4. partial_ordering: double i NaN --\n";
    double nan = std::nan("");
    std::partial_ordering r = 1.0 <=> nan;
    bool unordered = (r == std::partial_ordering::unordered);
    std::cout << std::boolalpha << "  1.0 <=> NaN je unordered: " << unordered
              << "; 1.0 < NaN: " << (1.0 < nan) << ", 1.0 > NaN: " << (1.0 > nan) << ", 1.0 == NaN: " << (1.0 == nan)
              << "\n  <- zato je <=> za strukturu sa double članom partial_ordering\n" << std::noboolalpha;
}

int main() {
    s01_defaultedSpaceship();
    s02_customSpaceship();
    s03_rewrittenCandidates();
    s04_partialOrdering();
}
