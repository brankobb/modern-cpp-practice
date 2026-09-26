// Rešenje zadatka ex1_shapes.

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Korak 1: interfejs = apstraktna klasa (bar jedna čista virtualna
// funkcija). Virtualni destruktor: objekti se brišu preko Shape* (unique_ptr
// u vektoru), pa mora da se pozove destruktor IZVEDENE klase (ub/u01).
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
    // Korak 3: "virtuelni konstruktor kopije" (C.130).
    virtual std::unique_ptr<Shape> clone() const = 0;

    Shape& operator=(const Shape&) = delete;

protected:
    // Korak 3: protected -- izvedene klase mogu da se kopiraju (za clone),
    // ali "Shape s = circle;" (slicing) spolja ne prolazi.
    Shape() = default;
    Shape(const Shape&) = default;
};

// Korak 2: override -- kompajler proveri da zaista nadjačava (zadatak ex2).
class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; }
    std::string name() const override { return "circle"; }
    std::unique_ptr<Shape> clone() const override { return std::make_unique<Circle>(*this); }

private:
    double r_;
};

class Rectangle : public Shape {
public:
    Rectangle(double a, double b) : a_(a), b_(b) {}
    double area() const override { return a_ * b_; }
    std::string name() const override { return "rectangle"; }
    std::unique_ptr<Shape> clone() const override { return std::make_unique<Rectangle>(*this); }

private:
    double a_;
    double b_;
};

// final: od Square se dalje ne nasleđuje, a pozivi preko Square& mogu da
// se devirtualizuju (sekcija 7).
class Square final : public Rectangle {
public:
    explicit Square(double a) : Rectangle(a, a) {}
    std::string name() const override { return "square"; }
    std::unique_ptr<Shape> clone() const override { return std::make_unique<Square>(*this); }
};

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(1.0));
    shapes.push_back(std::make_unique<Rectangle>(2.0, 3.0));
    shapes.push_back(std::make_unique<Square>(4.0));
    double total = 0;
    for (const auto& s : shapes) {
        std::cout << s->name() << ": " << s->area() << '\n';
        total += s->area();
    }
    std::cout << "total: " << total << '\n';

    std::unique_ptr<Shape> copy = shapes[2]->clone();
    std::cout << "copy: " << copy->name() << ' ' << copy->area() << '\n';
}
