#include <iostream>
#include <vector>

class Shape { // abstraktna -- ima pure virtual funkciju
public:
    virtual double area() const = 0;
    virtual void describe() const {
        std::cout << "Shape sa površinom " << area() << "\n";
    }
    virtual ~Shape() = default; // virtual destructor -- podsetnik iz week1 s01
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    // Ako izbaciš override (ili napraviš tipfeler u potpisu, npr.
    // izbaciš const) (NIJE DOBRO, tih bug) jer bi kompajler tiho napravio
    // NOVU, nepovezanu funkciju umesto da override-uje Shape::area() --
    // greška postaje vidljiva tek u runtime-u (ili, kao ovde, čak ni
    // tad -- klasa ostaje abstraktna i ne kompajlira, što je BAR neki
    // signal; kod običnih virtual funkcija BEZ = 0 ova greška je
    // POTPUNO tiha).
    // Treba da UVEK pišeš override na funkcijama koje override-uju --
    // kompajler onda proveri da se potpisi STVARNO poklapaju.
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

int main() {
    // Ako probaš Shape s; (NIJE DOBRO, ne kompajlira) jer Shape ima pure
    // virtual funkciju (area() = 0) -- to čini klasu abstraktnom, ne
    // može se instancirati direktno.
    // Shape s; // TODO: otkomentariši -- compile error, Shape je abstraktna

    std::cout << "-- polimorfizam preko Shape* --\n";
    std::vector<Shape*> shapes;
    shapes.push_back(new Circle(2.0));
    shapes.push_back(new Rectangle(3.0, 4.0));

    for (const Shape* s : shapes) {
        s->describe(); // dynamic dispatch -- poziva PRAVU area() za svaki tip
    }

    // Ako zaboraviš delete (NIJE DOBRO) jer curi memorija za svaki
    // new-ovani Shape -- ASan bi ovo prijavio kao memory leak.
    // Treba da ručno oslobodiš svaki new-ovani objekat kad ga više ne
    // koristiš, kao ispod.
    // Možeš i (bolje!) koristiti std::vector<std::unique_ptr<Shape>>
    // umesto sirovih pokazivača -- tad se delete dešava AUTOMATSKI kad
    // unique_ptr izađe iz scope-a, nema šanse da zaboraviš (videćeš u
    // week2 s08).
    for (Shape* s : shapes) delete s; // virtual dtor -- ispravno brisanje kroz Base*
}
