// STD: c++17
// EXPECT-GCC: mem-initializer for 'Rect::w_' follows constructor delegation
// EXPECT-CLANG: an initializer for a delegating constructor must appear alone
// POGREŠNO: delegirajući konstruktor koji uz to inicijalizuje i član.
// Zašto: ciljni konstruktor (Rect(1, 1)) već inicijalizuje SVE članove.
//   Da w_ može da se inicijalizuje i ovde, bio bi inicijalizovan dvaput
//   ([class.base.init]: delegacija mora biti jedini inicijalizator).
// Ispravno: delegiraj sa pravim vrednostima (Rect() : Rect(2, 1) {}), ili
//   promeni vrednost u TELU konstruktora, posle delegacije.
class Rect {
public:
    Rect(int w, int h) : w_(w), h_(h) {}
    Rect() : Rect(1, 1), w_(2) {}
    int area() const { return w_ * h_; }

private:
    int w_;
    int h_;
};

int main() {
    Rect r;
    return r.area();
}
