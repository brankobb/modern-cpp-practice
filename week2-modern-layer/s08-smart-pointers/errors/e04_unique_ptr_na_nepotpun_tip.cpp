// STD: c++17
// EXPECT-GCC: invalid application of 'sizeof' to incomplete type 'Widget::Impl'
// EXPECT-CLANG: invalid application of 'sizeof' to an incomplete type 'Widget::Impl'
// POGREŠNO: pimpl sa unique_ptr, a destruktor Widget-a nije deklarisan.
// Zašto: kompajler napiše ~Widget() INLINE, tamo gde je Impl samo
//   deklarisan (nepotpun tip). ~unique_ptr<Impl> tada treba da obriše Impl,
//   a za to mora da zna njegovu veličinu i destruktor; std::default_delete
//   to proverava static_assert-om (EMC Item 22).
// Ispravno: u header-u samo deklaracija ~Widget(); a u .cpp, POSLE
//   definicije struct Impl: Widget::~Widget() = default; (isto za move).
#include <memory>

class Widget {
public:
    Widget();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

int main() {
    Widget w;
}
