// STD: c++20
// EXPECT-GCC: no match for 'operator==' (operand types are 'Version' and 'Version')
// EXPECT-CLANG: invalid operands to binary expression ('Version' and 'Version')
// POGREŠNO: ručno napisan operator<=>, a zatim a == b.
// Zašto: iz <=> se prepisuju samo <, >, <= i >=. == se NE izvodi iz
//   ručno napisanog <=>, jer često može brže (npr. stringovi različite
//   dužine su odmah različiti, bez poređenja znak po znak). Samo
//   "= default" za <=> implicitno daje i defaulted ==.
// Ispravno: dodaj bool operator==(const Version&) const (= default ili
//   ručno), ili "auto operator<=>(const Version&) const = default;".
#include <compare>

struct Version {
    int major;
    int minor;
    std::strong_ordering operator<=>(const Version& o) const {
        if (auto c = major <=> o.major; c != 0) return c;
        return minor <=> o.minor;
    }
};

int main() {
    Version a{1, 2};
    Version b{1, 3};
    bool less = a < b;
    bool equal = a == b;
    return less && !equal;
}
