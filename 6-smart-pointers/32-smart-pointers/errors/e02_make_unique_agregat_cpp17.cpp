// STD: c++17
// EXPECT-GCC: new initializer expression list treated as compound expression
// EXPECT-CLANG: no matching constructor for initialization of 'Point'
// POGREŠNO (u C++17): std::make_unique<Point>(1, 2) za agregat bez konstruktora.
// Zašto: make_unique radi "new Point(1, 2)" -- sa ZAGRADAMA. Agregat nema
//   konstruktor sa dva argumenta, a do C++20 se agregat nije mogao
//   inicijalizovati zagradama (lekcija 03). C++20 (P0960) to dozvoljava,
//   pa se isti kod tamo kompajlira. g++ poruka je zbunjujuća: "1, 2" čita
//   kao operator zarez.
// Ispravno u C++17: std::make_unique<Point>(Point{1, 2}), ili konstruktor
//   Point(int x, int y) u strukturi.
#include <memory>

struct Point {
    int x;
    int y;
};

int main() {
    auto p = std::make_unique<Point>(1, 2);
    return p->x;
}
