// EXPECT-GCC: no match for 'operator<' (operand types are 'Tacka' and 'const std::array<Tacka, 2>::value_type'
// EXPECT-CLANG: invalid operands to binary expression ('Tacka' and 'const value_type'
// POGREŠNO TEK PRI POZIVU: metode klasnog šablona se instanciraju tek kad
// se POZOVU ([temp.inst]). Stek<Tacka> je ispravan tip -- push i pop rade
// (main.cpp, sekcija 3). Greška nastaje tek kad se pozove najveci(), jer
// ona traži operator<, a Tacka ga nema.
// Ispravno: ne zovi najveci() za tip bez poređenja, ili daj Tacka
// operator< (ako ima smisla), ili najveci(poredi) koji prima funkciju
// poređenja, kao std::max_element.
#include <array>
#include <cstddef>
template <typename T, std::size_t N>
class Stek {
public:
    void push(const T& v) { podaci_[vel_++] = v; }
    T najveci() const {
        T m = podaci_[0];
        for (std::size_t i = 1; i < vel_; ++i)
            if (m < podaci_[i]) m = podaci_[i];
        return m;
    }

private:
    std::array<T, N> podaci_{};
    std::size_t vel_ = 0;
};
struct Tacka {
    int x = 0, y = 0;
};
int main() {
    Stek<Tacka, 2> s;
    s.push({1, 2});
    return s.najveci().x;
}
