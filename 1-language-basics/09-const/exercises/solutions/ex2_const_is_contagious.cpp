// Rešenje zadatka ex2_const_is_contagious.

#include <iostream>
#include <map>
#include <string>

// Ako metode koje samo čitaju nisu const (nije dobro): objekat je
// neupotrebljiv preko const& i const pokazivača, a const se ne može dodati
// "kasnije samo na jedno mesto" -- svaka funkcija koja prima const& traži
// da i sve što poziva bude const.
// Treba ovako: sve što ne menja stanje je const od prvog dana.
class Config {
public:
    void set(const std::string& k, int v) { m_[k] = v; }        // menja: nije const
    int get(const std::string& k) const { return m_.at(k); }    // at(), ne []:
    std::size_t count() const { return m_.size(); }            // [] na mapi ubacuje ključ,
                                                                // pa nema const verziju
private:
    std::map<std::string, int> m_;
};

// Korak 3: Config& bi tražila promenljiv objekat -- ne bi primila ni
// const objekat ni privremeni, i ne bi obećala da ga ne menja. Po vrednosti
// bi se kopirala cela mapa pri svakom pozivu. const& je jedino što i ne
// kopira i prima sve.
void print(const Config& c) {
    std::cout << c.count() << " keys, baud=" << c.get("baud") << '\n';
}

int main() {
    Config c;
    c.set("baud", 115200);
    c.set("parity", 0);
    print(c);
}
