// Rešenje zadatka ex3_not_a_c_string.

#include <cstdio>
#include <string>
#include <string_view>

void sendToPort(const char* message) { std::printf("sent: [%s]\n", message); }   // "C API"

// Ako se prosledi deo.data() (nije dobro): C funkcija čita do '\0', a on
// je tek na kraju originala -- pošalje se i ostatak.
// Treba ovako: kopija u std::string ima svoj '\0' odmah posle dela.
// (Za funkcije koje primaju i dužinu nema kopije: std::fwrite(deo.data(), 1, deo.size(), f),
// ili printf("%.*s", static_cast<int>(deo.size()), deo.data()).)
void send(std::string_view part) { sendToPort(std::string(part).c_str()); }

int main() {
    std::string config = "temp=21;humidity=40";
    std::string_view all = config;
    auto sep = all.find(';');
    send(all.substr(0, sep));
    send(all.substr(sep + 1));
}
