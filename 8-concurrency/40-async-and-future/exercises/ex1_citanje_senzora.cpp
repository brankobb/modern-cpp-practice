// KIND: usage
// SANITIZER: thread
//
// Zadatak 1 -- paralelno čitanje senzora preko std::async, izuzeci kroz
// future, čekanje sa rokom (sekcije 1, 4, 6)
//   ./build.sh 8-concurrency/40-async-and-future/exercises/ex1_citanje_senzora.cpp
//   ./build.sh 8-concurrency/40-async-and-future/exercises/ex1_citanje_senzora.cpp --tsan
// Fajl se kompajlira i ovakav. Uradi korake redom, otkomentariši test u
// main() i uporedi izlaz sa blokom EXPECTED OUTPUT na dnu fajla.
// Rešenje: exercises/solutions/ex1_citanje_senzora.cpp
//
// Korak 1: std::vector<std::future<Merenje>> pokreniCitanja(const std::vector<std::string>& senzori)
//   -- za svaki senzor std::async(std::launch::async, citajSenzor, ime).
//   (Zašto ovde ne treba std::ref, a ne bi ni smeo?)
// Korak 2: void skupi(std::vector<std::future<Merenje>>& zadaci)
//   -- get() svakog u try/catch; ispiši "ime = vrednost" ili
//   "greška: what()", pa "uspelo N od M". Zadatak koji je bacio izuzetak
//   ne sme da obori ostale.
// Korak 3: std::optional<int> saRokom(std::future<int>& f, std::chrono::milliseconds rok)
//   -- wait_for; ako nije ready, std::nullopt, inače f.get().
//   Test koristi promise umesto async-a: future iz async-a koji nikad ne
//   završi bi u destruktoru čekao zauvek (sekcija 4).

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

// TODO korak 1, 2, 3

int main() {
    // Korak 1 i 2 -- otkomentariši:
    // auto zadaci = pokreniCitanja({"temp", "vlaga", "pritisak"});
    // skupi(zadaci);

    // Korak 3 -- otkomentariši:
    // std::promise<int> brz, spor;
    // std::future<int> fBrz = brz.get_future(), fSpor = spor.get_future();
    // brz.set_value(7);                       // spor se nikad ne postavi
    // auto a = saRokom(fBrz, std::chrono::milliseconds(20));
    // auto b = saRokom(fSpor, std::chrono::milliseconds(20));
    // std::cout << "brz: " << (a ? std::to_string(*a) : "isteklo") << ", spor: " << (b ? std::to_string(*b) : "isteklo")
    //           << '\n';
}

/* EXPECTED OUTPUT
temp = 40
greška: vlaga: nema odgovora
pritisak = 80
uspelo 2 od 3
brz: 7, spor: isteklo
*/
