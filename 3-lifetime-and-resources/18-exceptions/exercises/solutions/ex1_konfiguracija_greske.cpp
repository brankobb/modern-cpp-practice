// Rešenje zadatka ex1_konfiguracija_greske.

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Korak 1: nasledi runtime_error -- on čuva poruku (i kopira se bez
// bacanja), a what() je virtual, pa radi i preko std::exception&.
class GreskaKonfiguracije : public std::runtime_error {
public:
    GreskaKonfiguracije(int red, const std::string& poruka)
        : std::runtime_error("red " + std::to_string(red) + ": " + poruka), red_(red) {}
    int red() const noexcept { return red_; }

private:
    int red_;
};

// Korak 2: prevođenje -- detalj implementacije (stoi) ne curi napolje.
int procitajVrednost(const std::string& linija, int red) {
    auto jednako = linija.find('=');
    if (jednako == std::string::npos) throw GreskaKonfiguracije(red, "nema '='");
    std::string tekst = linija.substr(jednako + 1);
    try {
        return std::stoi(tekst);
    } catch (const std::logic_error&) {      // invalid_argument i out_of_range
        throw GreskaKonfiguracije(red, "vrednost nije broj: '" + tekst + "'");
    }
}

// Korak 3: kontekst ("šta smo radili") dodaje se na svakom nivou, a
// originalni uzrok ostaje unutra.
int ucitaj(const std::vector<std::string>& linije) {
    try {
        int zbir = 0;
        for (std::size_t i = 0; i < linije.size(); ++i)
            zbir += procitajVrednost(linije[i], static_cast<int>(i) + 1);
        return zbir;
    } catch (...) {
        std::throw_with_nested(std::runtime_error("konfiguracija nije učitana"));
    }
}

void ispisiLanac(const std::exception& e, int nivo = 0) {
    std::cout << std::string(static_cast<std::size_t>(nivo) * 2, ' ') << e.what() << '\n';
    try {
        std::rethrow_if_nested(e);
    } catch (const std::exception& unutra) {
        ispisiLanac(unutra, nivo + 1);
    }
}

int main() {
    std::cout << "zbir: " << ucitaj({"a=10", "b=20", "c=30"}) << '\n';
    for (const std::vector<std::string>& konfig :
         {std::vector<std::string>{"a=10", "b=abc"}, std::vector<std::string>{"a=10", "b 20"}}) {
        try {
            ucitaj(konfig);
        } catch (const std::exception& e) {
            ispisiLanac(e);
        }
    }
}
