// STD: c++17
// EXPECT-GCC: marked 'override', but does not override
// EXPECT-CLANG: non-virtual member function marked 'override' hides virtual member function
// POGREŠNO: override funkcija kojoj fali const.
// Zašto: area() i area() const su RAZLIČITE funkcije (const je deo potpisa).
//   Bez override bi se ovo kompajliralo: Circle bi dobio novu funkciju, a
//   poziv kroz const Shape& bi i dalje zvao Shape::area (tiho, bez greške;
//   g++ i clang sa -Wall bar upozore: -Woverloaded-virtual). Sa override
//   kompajler proveri da funkcija zaista nešto nadjačava.
// Ispravno: double area() const override. Uvek piši override (C.128).
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const { return 0; }
};

class Circle : public Shape {
public:
    double area() override { return 3.14; }
};

int main() {}
