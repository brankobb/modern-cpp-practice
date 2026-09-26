// Rešenje zadatka ex2_optional_bool.

#include <iostream>
#include <optional>
#include <string>

std::optional<bool> readSensor(char door) {
    if (door == 'A') return true;
    if (door == 'B') return false;
    return std::nullopt;
}

// Ako se proverava samo if (o) (nije dobro): to je has_value(), pa je
// optional<bool> = false "tačno" -- otvorena vrata izgledaju zatvoreno.
// Treba ovako: prvo prisustvo (has_value), pa sama vrednost (*o).
std::string describe(std::optional<bool> o) {
    if (!o.has_value()) return "UNKNOWN";
    return *o ? "CLOSED" : "OPEN";
}

int main() {
    for (char v : {'A', 'B', 'C'}) std::cout << "door " << v << ": " << describe(readSensor(v)) << '\n';
}
