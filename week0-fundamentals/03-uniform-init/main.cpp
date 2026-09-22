#include <iostream>
#include <vector>

struct Widget {
    Widget() { std::cout << "Widget()\n"; }
    Widget(int) { std::cout << "Widget(int)\n"; }
    Widget(std::initializer_list<int>) { std::cout << "Widget(initializer_list)\n"; }
};

int main() {
    // int narrow{3.14}; // TODO: otkomentariši -- treba compile error (narrowing)
    int narrow_ok(3.14); // ovo prolazi (uz warning) -- uporedi sa gornjim

    Widget w1();  // TODO: ovo je funkcija koja vraća Widget, NE objekat!
                  // proveri: std::cout << typeid(w1).name(); ne kompajlira jer w1 je funkcija
    Widget w2{};  // ovo JESTE default-konstruisan objekat

    Widget w3(5);   // poziva Widget(int)
    Widget w4{5};   // poziva Widget(initializer_list) jer postoji -- iznenađenje!

    // klasični primer: (3, 5) vs {3, 5}
    std::vector<int> va(3, 5); // 3 elementa, svaki = 5 -> [5, 5, 5]
    std::vector<int> vb{3, 5}; // initializer_list -> [3, 5]
    std::cout << "va.size()=" << va.size() << " vb.size()=" << vb.size() << "\n";

    (void)narrow_ok;
    (void)w3;
    (void)w4;
}
