// EXPECT-GCC: use of deleted function
// EXPECT-CLANG: call to implicitly-deleted default constructor of 'std::unordered_set<Point>'
// POGREŠNO: unordered_set<Point> koristi std::hash<Point>, a standard ne
// zna kako da hešira tvoj tip -- std::hash<Point> ne postoji (tj. obrisan
// je), pa ni kontejner ne može da se napravi.
// Ispravno: heš kao funkcijski objekat, drugi argument šablona:
//   std::unordered_set<Point, PointHash>   (main.cpp, sekcija 5)
// ili specijalizacija template <> struct std::hash<Point> { ... };
// Uz heš treba i operator== (ili treći argument, poređenje jednakosti).
#include <unordered_set>
struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};
int main() {
    std::unordered_set<Point> u;
    u.insert({1, 2});
    return static_cast<int>(u.size());
}
