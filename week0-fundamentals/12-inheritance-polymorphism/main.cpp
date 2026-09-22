#include <iostream>
#include <vector>

class Shape { // abstraktna -- ima pure virtual funkciju
public:
    virtual double area() const = 0;
    virtual void describe() const {
        // NE zovi area() ovde da demonstrira polimorfizam pravilno --
        // describe() se poziva POSLE pune konstrukcije, iz main-a, pa je OK
        std::cout << "Shape sa površinom " << area() << "\n";
    }
    virtual ~Shape() = default; // virtual destructor -- podsetnik iz week1 s01
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; } // override hvata tipfeler

private:
    double r_;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }

private:
    double w_, h_;
};

// TODO probaj: promeni "double area() const override" u Circle u
// "double area() override" (izbaci const) -- override će prijaviti GREŠKU
// jer se potpis ne poklapa sa Shape::area() const. Bez override-a bi ovo
// tiho napravilo novu funkciju koja se NIKAD ne bi pozvala polimorfno.

int main() {
    // Shape s; // TODO: otkomentariši -- compile error, Shape je abstraktna

    std::vector<Shape*> shapes;
    shapes.push_back(new Circle(2.0));
    shapes.push_back(new Rectangle(3.0, 4.0));

    for (const Shape* s : shapes) {
        s->describe(); // dynamic dispatch -- poziva PRAVU area() za svaki tip
    }

    for (Shape* s : shapes) delete s; // virtual dtor -- ispravno brisanje kroz Base*
    // (u week2 s08 ćeš ovo zameniti sa std::unique_ptr<Shape> da ne moraš ručno delete)
}
