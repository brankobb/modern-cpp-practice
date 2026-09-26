// STD: c++17
// EXPECT-GCC: no matching function for call to 'Widget::Widget()'
// EXPECT-CLANG: no matching constructor for initialization of 'Widget[3]'
// POGREŠNO: new Widget[3] za klasu koja nema podrazumevani konstruktor.
// Zašto: new T[n] bez inicijalizatora pravi svaki element konstruktorom
//   bez argumenata. Widget ga nema, jer ima korisnički konstruktor Widget(int).
// Ispravno: navedi vrednosti (new Widget[3]{Widget(1), Widget(2), Widget(3)}),
//   ili std::vector<Widget> sa reserve() i emplace_back(), koji pravi elemente
//   tek kad ih dodaš.
struct Widget {
    explicit Widget(int v) : value(v) {}
    int value;
};

int main() {
    Widget* w = new Widget[3];
    delete[] w;
}
