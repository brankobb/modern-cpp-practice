// STD: c++17
// EXPECT-GCC: use of deleted function 'Config::Config()'
// EXPECT-CLANG: call to implicitly-deleted default constructor of 'Config'
// POGREŠNO: "Config() = default;" za klasu sa const članom bez vrednosti.
// Zašto: "= default" traži od kompajlera da napravi konstruktor, ali on ne
//   može da inicijalizuje const int bez vrednosti, pa je konstruktor
//   OBRISAN ([class.default.ctor]). Greška se javi tek kad se objekat
//   pravi, a ne na liniji "= default". clang upozori već na toj liniji.
//   Isto važi za reference kao članove.
// Ispravno: podrazumevana vrednost u deklaraciji (const int limit_ = 10;),
//   ili konstruktor koji je postavlja kroz init listu.
class Config {
public:
    Config() = default;
    int limit() const { return limit_; }

private:
    const int limit_;
};

int main() {
    Config c;
    return c.limit();
}
