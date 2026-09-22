#include <iostream>
#include <memory>
#include <string>
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

// ---------------------------------------------------------------- slicing
// Kad se izvedeni objekat KOPIRA u objekat bazne klase, kopira se samo
// bazni deo -- izvedeni deo (podaci i override-i) se "odseca". Kompajler
// ne javlja ni grešku ni upozorenje.
class Animal {
public:
    virtual ~Animal() = default;
    virtual std::string speak() const { return "..."; }
};

class Dog : public Animal {
public:
    explicit Dog(std::string name) : name_(std::move(name)) {}
    std::string speak() const override { return name_ + ": Av!"; }

private:
    std::string name_; // ovaj podatak se gubi pri slicing-u
};

std::string byValue(Animal a) { return a.speak(); }          // kopira SAMO Animal deo
std::string byRef(const Animal& a) { return a.speak(); }     // bez kopije -- pravi tip ostaje
std::string byPtr(const Animal* a) { return a ? a->speak() : "(nema životinje)"; }

// Rešenje (C++ Core Guidelines C.67): polimorfna bazna klasa zabrani javno
// kopiranje. Tada se slicing NE KOMPAJLIRA (errors/e01), umesto da tiho radi.
class SafeAnimal {
public:
    SafeAnimal() = default;
    SafeAnimal(const SafeAnimal&) = delete;
    SafeAnimal& operator=(const SafeAnimal&) = delete;
    virtual ~SafeAnimal() = default;
    virtual std::string speak() const { return "..."; }
};

class SafeDog : public SafeAnimal {
public:
    std::string speak() const override { return "Av!"; }
};

std::string describe(const SafeAnimal& a) { return a.speak(); } // jedino što radi: referenca

void slicingDemo() {
    std::cout << "-- slicing --\n";
    Dog rex("Rex");

    std::cout << "byValue(rex): " << byValue(rex) << "   <- odsečeno na Animal\n";
    std::cout << "byRef(rex):   " << byRef(rex) << "\n";
    std::cout << "byPtr(&rex):  " << byPtr(&rex) << "   byPtr(nullptr): " << byPtr(nullptr) << "\n";

    Animal copy = rex; // kopija u bazni objekat -- ista stvar
    std::cout << "Animal copy = rex; copy.speak(): " << copy.speak() << "\n";

    std::vector<Animal> zoo;
    zoo.push_back(rex); // kontejner baznih objekata čuva samo bazne delove
    std::cout << "vector<Animal>[0].speak(): " << zoo[0].speak() << "\n";

    std::vector<std::unique_ptr<Animal>> realZoo; // ispravno: čuvaj pokazivače
    realZoo.push_back(std::make_unique<Dog>("Max"));
    std::cout << "vector<unique_ptr<Animal>>[0]->speak(): " << realZoo[0]->speak() << "\n";

    SafeDog safe;
    std::cout << "describe(SafeDog): " << describe(safe) << " (kopija se ne kompajlira)\n";
}

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

    slicingDemo();
}
