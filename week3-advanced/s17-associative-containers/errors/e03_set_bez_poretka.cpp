// EXPECT-GCC: no match for 'operator<' (operand types are 'const Tacka' and 'const Tacka')
// EXPECT-CLANG: invalid operands to binary expression ('const Tacka' and 'const Tacka')
// POGREŠNO: set<Tacka> koristi std::less<Tacka>, tj. operator<. Tacka ga
// nema. (Samo tip bez poretka je u redu -- greška nastaje tek kad nešto
// mora da se uporedi, pri insert-u.)
// Ispravno: operator< za Tacka (npr. std::tie(x, y) < std::tie(o.x, o.y),
// lekcija 12, zadatak z3), ili poredak kao drugi argument šablona:
// std::set<Tacka, PoredakTacaka>.
#include <set>
struct Tacka {
    int x, y;
};
int main() {
    std::set<Tacka> s;
    s.insert({1, 2});
    return static_cast<int>(s.size());
}
