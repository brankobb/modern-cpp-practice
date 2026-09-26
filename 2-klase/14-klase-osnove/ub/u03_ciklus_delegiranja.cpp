// ONLY-CC: g++
// EXPECT-UB: stack-overflow
// POGREŠNO: dva konstruktora delegiraju jedan drugom.
// Zašto: Rect() poziva Rect(1), koji poziva Rect(), i tako unedogled, dok se
//   stek ne prepuni. Standard kaže da je takav program neispravan, ali ne
//   traži dijagnostiku. clang ga odbija (-Wdelegating-ctor-cycles je greška
//   po podrazumevanom), a g++ ga kompajlira bez upozorenja. Direktan ciklus
//   odbijaju oba (errors/e07).
// Ispravno: jedan "glavni" konstruktor koji ne delegira, ostali delegiraju
//   njemu (main.cpp, sekcija 9).
#include <cstdio>

class Rect {
public:
    Rect() : Rect(1) {}
    explicit Rect(int side) : Rect() { side_ = side; }
    int side() const { return side_; }

private:
    int side_ = 0;
};

int main() {
    Rect r;
    std::printf("%d\n", r.side());
}
