// STD: c++17
// EXPECT-GCC: use of deleted function 'Widget::Widget(Widget&&)'
// EXPECT-CLANG: call to deleted constructor of 'Widget'
// POGREŠNO: "Widget(Widget&&) = delete;" na klasi koja se kopira.
// Zašto: obrisana funkcija UČESTVUJE u overload resolution-u (lekcija 11).
//   return w; prvo pokušava w kao rvalue, pa pobedi Widget(Widget&&) --
//   koji je obrisan, i to je greška, a ne povratak na kopiju. Isto za
//   Widget b = std::move(a) i za realokaciju vektora.
// Ispravno: move se ne briše. Ako tip ne treba da se pomera, ne deklariši
//   move uopšte (copy ctor će ga zameniti), a ako ne treba ni da se kopira,
//   obriši copy (move tada ni ne postoji).
#include <string>

struct Widget {
    std::string name;
    Widget() = default;
    Widget(const Widget&) = default;
    Widget(Widget&&) = delete;
};

Widget make() {
    Widget w;
    w.name = "x";
    return w;
}

int main() {
    Widget w = make();
    (void)w;
}
