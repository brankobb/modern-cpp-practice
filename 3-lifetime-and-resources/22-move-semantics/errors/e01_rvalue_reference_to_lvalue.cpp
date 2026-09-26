// STD: c++17
// EXPECT-GCC: cannot bind rvalue reference of type 'std::string&&'
// EXPECT-CLANG: rvalue reference to type 'basic_string<...>' cannot bind to lvalue of type 'basic_string<...>'
// POGREŠNO: T&& vezan za lvalue (promenljivu sa imenom).
// Zašto: T&& znači "ovaj objekat niko više ne koristi, sme da se isprazni".
//   name se koristi i posle ove linije, pa bi pražnjenje bilo iznenađenje.
//   Jezik zato dozvoljava T&& samo za privremene objekte i za ono što je
//   EKSPLICITNO označeno sa std::move.
// Ispravno: std::string&& r = std::move(name); (i posle toga ne oslanjaj se
//   na sadržaj name), ili const std::string& r = name; ako samo čitaš.
#include <string>

int main() {
    std::string name = "Ann";
    std::string&& r = name;
    return static_cast<int>(r.size());
}
