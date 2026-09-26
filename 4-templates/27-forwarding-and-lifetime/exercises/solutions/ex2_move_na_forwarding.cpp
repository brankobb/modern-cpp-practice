// Rešenje zadatka ex2_move_na_forwarding.

#include <iostream>
#include <string>
#include <utility>

// Ako na forwarding referenci pišeš std::move (nije dobro): lvalue
// pozivaoca se pomeri, i on ostane sa praznim (moved-from) objektom.
// Treba ovako: std::forward<T> -- pomeri samo ako je argument bio rvalue.
struct Uredjaj {
    std::string ime;
    template <typename T>
    void postaviIme(T&& novo) { ime = std::forward<T>(novo); }
};

int main() {
    Uredjaj u;
    std::string ime = "senzor-temperature-01";
    u.postaviIme(ime);
    std::cout << "uređaj: " << u.ime << '\n';
    std::cout << "ime pozivaoca posle: \"" << ime << "\"\n";
    u.postaviIme(std::string("senzor-pritiska-02"));
    std::cout << "uređaj: " << u.ime << '\n';
}
