// STD: c++17
// EXPECT-GCC: no matching function for call to 'Widget::Widget()'
// EXPECT-CLANG: no matching constructor for initialization of 'Widget'
// POGREŠNO: "Widget w;" kada klasa ima samo konstruktor sa parametrom.
// Zašto: kompajler sam pravi podrazumevani konstruktor SAMO ako klasa nema
//   nijedan korisnički konstruktor ([class.default.ctor]). Čim napišeš
//   Widget(int), podrazumevani nestaje.
// Ispravno: Widget w(5); ili vrati podrazumevani sa "Widget() = default;"
//   (uz podrazumevanu vrednost člana: int value_ = 0;).
class Widget {
public:
    explicit Widget(int v) : value_(v) {}
    int value() const { return value_; }

private:
    int value_;
};

int main() {
    Widget w;
    return w.value();
}
