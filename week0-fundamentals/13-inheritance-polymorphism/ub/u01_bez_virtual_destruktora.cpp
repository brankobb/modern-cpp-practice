// EXPECT-UB: new-delete-type-mismatch
// POGREŠNO: delete kroz Shape*, a ~Shape() nije virtual (EC++ Item 7).
// Zašto: delete poziva destruktor STATIČKOG tipa (Shape). ~Label se ne
//   pozove, pa std::string text_ ne oslobodi svoju memoriju, a operator
//   delete dobije pogrešnu veličinu objekta. Standard: UB ([expr.delete]).
//   ASan to vidi kao razliku u veličini (sizeof(Label) != sizeof(Shape)).
//   Kad se ta provera isključi, LeakSanitizer prijavi 101 bajt: tekst koji
//   ~Label nije oslobodio.
// Ispravno: virtual ~Shape() = default; u svakoj klasi namenjenoj
//   polimorfnom korišćenju (C.35: javni virtual ili protected ne-virtual
//   destruktor).
#include <cstdio>
#include <string>

class Shape {
public:
    ~Shape() { std::puts("~Shape"); }
};

class Label : public Shape {
public:
    Label() : text_(100, 'x') {}
    ~Label() { std::puts("~Label"); }

private:
    std::string text_;
};

int main() {
    Shape* s = new Label;
    delete s;
}
