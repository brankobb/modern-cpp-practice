// STD: c++17
// EXPECT-GCC: constructor delegates to itself
// EXPECT-CLANG: constructor for 'Rect' creates a delegation cycle
// POGREŠNO: konstruktor koji delegira samom sebi.
// Zašto: delegacija koja se vraća na isti konstruktor je beskonačna
//   rekurzija; standard kaže da je program neispravan ([class.base.init]).
//   Oba kompajlera odbijaju DIREKTAN ciklus. Ciklus preko dva konstruktora
//   (A() -> A(int) -> A()) odbija samo clang; g++ ga kompajlira, a program
//   se sruši (ub/u03).
// Ispravno: jedan "glavni" konstruktor sa svim parametrima, ostali delegiraju
//   njemu, a on ne delegira nikome.
class Rect {
public:
    explicit Rect(int w) : Rect(w) {}
    int width() const { return w_; }

private:
    int w_ = 0;
};

int main() {
    Rect r(1);
    return r.width();
}
