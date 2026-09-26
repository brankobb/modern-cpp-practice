// Rešenje zadatka ex2_optional_bool.

#include <iostream>
#include <optional>
#include <string>

std::optional<bool> ocitaj(char vrata) {
    if (vrata == 'A') return true;
    if (vrata == 'B') return false;
    return std::nullopt;
}

// Ako se proverava samo if (o) (nije dobro): to je has_value(), pa je
// optional<bool> = false "tačno" -- otvorena vrata izgledaju zatvoreno.
// Treba ovako: prvo prisustvo (has_value), pa sama vrednost (*o).
std::string opis(std::optional<bool> o) {
    if (!o.has_value()) return "NEPOZNATO";
    return *o ? "ZATVORENA" : "OTVORENA";
}

int main() {
    for (char v : {'A', 'B', 'C'}) std::cout << "vrata " << v << ": " << opis(ocitaj(v)) << '\n';
}
