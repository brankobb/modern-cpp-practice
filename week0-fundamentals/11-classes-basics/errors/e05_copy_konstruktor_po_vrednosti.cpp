// STD: c++17
// EXPECT-GCC: invalid constructor; you probably meant 'Widget (const Widget&)'
// EXPECT-CLANG: copy constructor must pass its first argument by reference
// POGREŠNO: copy konstruktor koji prima parametar PO VREDNOSTI.
// Zašto: prosleđivanje po vrednosti pravi kopiju, a kopija se pravi copy
//   konstruktorom. Da bi pozvao Widget(Widget other), moraš prvo da napraviš
//   other, pozivom Widget(Widget other)... beskonačna rekurzija. Zato ga
//   standard zabranjuje ([class.copy.ctor]).
// Ispravno: Widget(const Widget& other).
class Widget {
public:
    Widget() = default;
    Widget(Widget other) : value_(other.value_) {}

private:
    int value_ = 0;
};

int main() {
    Widget a;
    Widget b = a;
    (void)b;
}
