// EXPECT-GCC: no match for 'operator<' (operand types are 'const Point' and 'const Point')
// EXPECT-CLANG: invalid operands to binary expression ('const Point' and 'const Point')
// POGREŠNO: set<Point> koristi std::less<Point>, tj. operator<. Point ga
// nema. (Samo tip bez poretka je u redu -- greška nastaje tek kad nešto
// mora da se uporedi, pri insert-u.)
// Ispravno: operator< za Point (npr. std::tie(x, y) < std::tie(o.x, o.y),
// lekcija 15, zadatak ex3), ili poredak kao drugi argument šablona:
// std::set<Point, PointOrder>.
#include <set>
struct Point {
    int x, y;
};
int main() {
    std::set<Point> s;
    s.insert({1, 2});
    return static_cast<int>(s.size());
}
