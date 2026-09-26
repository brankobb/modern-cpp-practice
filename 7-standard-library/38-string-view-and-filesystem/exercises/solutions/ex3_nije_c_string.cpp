// Rešenje zadatka ex3_nije_c_string.

#include <cstdio>
#include <string>
#include <string_view>

void posaljiNaPort(const char* poruka) { std::printf("poslato: [%s]\n", poruka); }   // "C API"

// Ako se prosledi deo.data() (nije dobro): C funkcija čita do '\0', a on
// je tek na kraju originala -- pošalje se i ostatak.
// Treba ovako: kopija u std::string ima svoj '\0' odmah posle dela.
// (Za funkcije koje primaju i dužinu nema kopije: std::fwrite(deo.data(), 1, deo.size(), f),
// ili printf("%.*s", static_cast<int>(deo.size()), deo.data()).)
void posalji(std::string_view deo) { posaljiNaPort(std::string(deo).c_str()); }

int main() {
    std::string konfig = "temp=21;vlaga=40";
    std::string_view sve = konfig;
    auto sep = sve.find(';');
    posalji(sve.substr(0, sep));
    posalji(sve.substr(sep + 1));
}
