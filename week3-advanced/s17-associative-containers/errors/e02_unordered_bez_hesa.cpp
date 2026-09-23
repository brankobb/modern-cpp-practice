// EXPECT-GCC: use of deleted function
// EXPECT-CLANG: call to implicitly-deleted default constructor of 'std::unordered_set<Tacka>'
// POGREŠNO: unordered_set<Tacka> koristi std::hash<Tacka>, a standard ne
// zna kako da hešira tvoj tip -- std::hash<Tacka> ne postoji (tj. obrisan
// je), pa ni kontejner ne može da se napravi.
// Ispravno: heš kao funkcijski objekat, drugi argument šablona:
//   std::unordered_set<Tacka, HesTacke>   (main.cpp, sekcija 5)
// ili specijalizacija template <> struct std::hash<Tacka> { ... };
// Uz heš treba i operator== (ili treći argument, poređenje jednakosti).
#include <unordered_set>
struct Tacka {
    int x, y;
    bool operator==(const Tacka& o) const { return x == o.x && y == o.y; }
};
int main() {
    std::unordered_set<Tacka> u;
    u.insert({1, 2});
    return static_cast<int>(u.size());
}
