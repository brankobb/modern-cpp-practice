// Rešenje zadatka z3_konstruktor_baca.

#include <iostream>
#include <stdexcept>
#include <string>

// Ako objekat pravi prazan pa ga "otvaraš" sa bool init() (nije dobro):
// postoji stanje "napravljen, a neispravan", i lako se zaboravi provera.
// Treba ovako: konstruktor uspostavi invarijantu ili baci. Neuspeli
// konstruktor ne ostavlja objekat, pa neispravan Port ne može da postoji.
class Port {
public:
    explicit Port(int broj) : broj_(broj) {
        if (broj < 0 || broj > 3)
            throw std::invalid_argument("port " + std::to_string(broj) + " ne postoji");
    }
    void posalji(const char* s) const { std::cout << "port " << broj_ << ": " << s << '\n'; }

private:
    int broj_;
};

// Korak 3: sa std::optional<Port> napraviPort(int) i [[nodiscard]],
// zaboravljena provera je "*opt bez if (opt)" -- kompajler ne tera
// proveru, ali bar vrednost ne može da se ignoriše, a prazan optional
// je vidljiv u tipu.

int main() {
    try {
        Port p(9);
        p.posalji("zdravo");
    } catch (const std::invalid_argument& e) {
        std::cout << "greška: " << e.what() << '\n';
    }
    Port ok(2);
    ok.posalji("zdravo");
}
