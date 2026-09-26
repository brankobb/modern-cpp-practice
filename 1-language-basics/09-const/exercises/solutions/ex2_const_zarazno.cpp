// Rešenje zadatka ex2_const_zarazno.

#include <iostream>
#include <map>
#include <string>

// Ako metode koje samo čitaju nisu const (nije dobro): objekat je
// neupotrebljiv preko const& i const pokazivača, a const se ne može dodati
// "kasnije samo na jedno mesto" -- svaka funkcija koja prima const& traži
// da i sve što poziva bude const.
// Treba ovako: sve što ne menja stanje je const od prvog dana.
class Konfiguracija {
public:
    void postavi(const std::string& k, int v) { m_[k] = v; }   // menja: nije const
    int uzmi(const std::string& k) const { return m_.at(k); }  // at(), ne []:
    std::size_t broj() const { return m_.size(); }             // [] na mapi ubacuje ključ,
                                                               // pa nema const verziju
private:
    std::map<std::string, int> m_;
};

// Korak 3: Konfiguracija& bi tražila promenljiv objekat -- ne bi primila ni
// const objekat ni privremeni, i ne bi obećala da ga ne menja. Po vrednosti
// bi se kopirala cela mapa pri svakom pozivu. const& je jedino što i ne
// kopira i prima sve.
void ispis(const Konfiguracija& c) {
    std::cout << c.broj() << " ključa, baud=" << c.uzmi("baud") << '\n';
}

int main() {
    Konfiguracija c;
    c.postavi("baud", 115200);
    c.postavi("parity", 0);
    ispis(c);
}
