// Rešenje zadatka z1_citanje_senzora.

#include <chrono>
#include <future>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

struct Merenje {
    std::string senzor;
    int vrednost;
};

// Dato: "čitanje" jednog senzora; baca za senzor koji ne odgovara.
Merenje citajSenzor(const std::string& ime) {
    if (ime == "vlaga") throw std::runtime_error("vlaga: nema odgovora");
    return {ime, static_cast<int>(ime.size()) * 10};
}

// Korak 1: jedan async zadatak po senzoru; ime se KOPIRA u zadatak.
// Sa std::ref(s) zadatak bi čitao element vektora senzori -- a u main-u
// je to privremeni vektor ({"temp", ...}), koji nestane na kraju iskaza,
// dok zadaci možda još rade. Kopija ne može da "istekne" (CP.31).
std::vector<std::future<Merenje>> pokreniCitanja(const std::vector<std::string>& senzori) {
    std::vector<std::future<Merenje>> f;
    for (const auto& s : senzori) f.push_back(std::async(std::launch::async, citajSenzor, s));
    return f;
}

// Korak 2: get() baci izuzetak iz zadatka -- ovde, u pozivaocu.
void skupi(std::vector<std::future<Merenje>>& zadaci) {
    int uspelo = 0;
    for (auto& z : zadaci) {
        try {
            Merenje m = z.get();
            std::cout << m.senzor << " = " << m.vrednost << '\n';
            ++uspelo;
        } catch (const std::exception& e) {
            std::cout << "greška: " << e.what() << '\n';
        }
    }
    std::cout << "uspelo " << uspelo << " od " << zadaci.size() << '\n';
}

// Korak 3: wait_for ne uzima rezultat; get samo ako je spreman.
std::optional<int> saRokom(std::future<int>& f, std::chrono::milliseconds rok) {
    if (f.wait_for(rok) != std::future_status::ready) return std::nullopt;
    return f.get();
}

int main() {
    auto zadaci = pokreniCitanja({"temp", "vlaga", "pritisak"});
    skupi(zadaci);

    std::promise<int> brz, spor;
    std::future<int> fBrz = brz.get_future(), fSpor = spor.get_future();
    brz.set_value(7);                       // spor se nikad ne postavi
    auto a = saRokom(fBrz, std::chrono::milliseconds(20));
    auto b = saRokom(fSpor, std::chrono::milliseconds(20));
    std::cout << "brz: " << (a ? std::to_string(*a) : "isteklo") << ", spor: " << (b ? std::to_string(*b) : "isteklo")
              << '\n';
}
