// STD: c++17
// EXPECT-GCC: use of deleted function 'Widget::Widget(const Widget&)'
// EXPECT-CLANG: call to implicitly-deleted copy constructor of 'Widget'
// POGREŠNO: klasa deklariše samo move dodelu, a kod je kopira.
// Zašto: deklarisana move operacija (konstruktor ILI dodela) briše OBE
//   copy operacije, a ne generiše se ni druga move operacija (tabela u
//   notes.md, EMC Item 17). Widget sada nema ni copy konstruktor ni move
//   konstruktor.
// Ispravno: ako ti treba jedna, deklariši svih pet (makar = default), ili
//   nijednu (rule of 0).
#include <string>
#include <utility>

struct Widget {
    std::string name;
    Widget() = default;
    Widget& operator=(Widget&& o) noexcept {
        name = std::move(o.name);
        return *this;
    }
};

int main() {
    Widget a;
    Widget b = a;
    (void)b;
}
