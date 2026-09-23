// EXPECT-GCC: T must occur exactly once in alternatives
// EXPECT-CLANG: T must occur exactly once in alternatives
// POGREŠNO: variant<int, std::string> nikad ne može da drži double, pa
// get<double> nema smisla -- greška pri kompajliranju, ne izuzetak.
// (get<int> dok drži string se kompajlira i baca bad_variant_access --
// runtime/r02.)
// Ispravno: pitaj za tip koji jeste alternativa: std::get<int>(v).
#include <string>
#include <variant>
int main() {
    std::variant<int, std::string> v = 5;
    return static_cast<int>(std::get<double>(v));
}
