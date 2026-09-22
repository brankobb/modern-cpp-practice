#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Nasleđivanje i polimorfizam -- ISPRAVNI slučajevi. Sve se kompajlira i
// radi bez ASan/UBSan prijava (g++ 13 i clang 18, C++17 i C++20).
// POGREŠNI slučajevi:
//   errors/  -- kod koji se NE kompajlira
//   ub/      -- kod koji se kompajlira, ali je undefined behavior
// ./check_cases.sh week0-fundamentals/13-inheritance-polymorphism  proverava oba.

// ---------------------------------------------------------------- 1
class Account {
public:
    explicit Account(std::string owner) : owner_(std::move(owner)) {}
    virtual ~Account() = default;
    const std::string& owner() const { return owner_; }
    int balance() const { return balance_; }
    void deposit(int amount) { balance_ += amount; }

protected:
    int balance_ = 0; // protected: vidi ga izvedena klasa, ali ne i ostatak programa

private:
    std::string owner_; // private: ne vidi ga ni izvedena klasa (errors/e06)
};

// public nasleđivanje = "is-a": SavingsAccount JESTE Account i može se
// koristiti svuda gde se očekuje Account (EC++ Item 32).
class SavingsAccount : public Account {
public:
    SavingsAccount(std::string owner, int ratePercent) : Account(std::move(owner)), ratePercent_(ratePercent) {}
    void addInterest() { balance_ += balance_ * ratePercent_ / 100; } // pristup protected članu

private:
    int ratePercent_;
};

int totalBalance(const std::vector<const Account*>& accounts) {
    int total = 0;
    for (const Account* a : accounts) total += a->balance();
    return total;
}

void s01_basics() {
    std::cout << "-- 1. public nasleđivanje: is-a --\n";
    SavingsAccount savings("Ana", 10);
    savings.deposit(1000);  // nasleđena funkcija
    savings.addInterest();  // sopstvena funkcija
    Account plain("Bojan");
    plain.deposit(50);
    // SavingsAccount* se sam pretvara u Account* (izvedena -> bazna).
    std::cout << "  " << savings.owner() << ": " << savings.balance() << ", ukupno preko const Account*: "
              << totalBalance({&savings, &plain}) << "\n";
}

// ---------------------------------------------------------------- 2
class Part {
public:
    explicit Part(const char* name) : name_(name) { std::cout << name_ << "() "; }
    ~Part() { std::cout << "~" << name_ << "() "; }

private:
    const char* name_;
};

class Vehicle {
public:
    explicit Vehicle(int wheels) : wheels_(wheels), frame_("frame") { std::cout << "Vehicle(" << wheels_ << ") "; }
    virtual ~Vehicle() { std::cout << "~Vehicle() "; }
    int wheels() const { return wheels_; }

private:
    int wheels_;
    Part frame_;
};

class Bike : public Vehicle {
public:
    Bike() : Vehicle(2), bell_("bell") { std::cout << "Bike() "; } // baza se inicijalizuje u init listi (errors/e09)
    ~Bike() override { std::cout << "~Bike() "; }

private:
    Part bell_;
};

class Truck : public Vehicle {
public:
    using Vehicle::Vehicle; // C++11: nasledi konstruktore baze (Truck(int))
};

void s02_constructionOrder() {
    std::cout << "-- 2. redosled konstrukcije i destrukcije --\n";
    std::cout << "  ";
    {
        Bike bike; // baza (sa svojim članovima) -> članovi izvedene -> telo izvedene
        std::cout << "| ";
    } // obrnuto: telo izvedene -> članovi izvedene -> baza
    std::cout << "\n  Truck(6) preko using Vehicle::Vehicle: ";
    {
        Truck truck(6);
        std::cout << "wheels=" << truck.wheels() << " | ";
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------- 3
class Logger {
public:
    virtual ~Logger() = default;
    std::string log(int value) const { return "Logger::log(int " + std::to_string(value) + ")"; }
    std::string log(const std::string& text) const { return "Logger::log(string " + text + ")"; }
};

class HidingLogger : public Logger {
public:
    // Novo ime log u izvedenoj klasi SAKRIJE SVE log iz baze, i one sa
    // drugim parametrima (errors/e08). Nije overload preko granice klase.
    std::string log(double value) const { return "HidingLogger::log(double " + std::to_string(value) + ")"; }
};

class FileLogger : public Logger {
public:
    using Logger::log; // vrati imena iz baze u ovaj scope -> sada su svi overload-i zajedno
    std::string log(double value) const { return "FileLogger::log(double " + std::to_string(value) + ")"; }
};

void s03_nameHiding() {
    std::cout << "-- 3. sakrivanje imena i using Base::f --\n";
    HidingLogger hiding;
    FileLogger file;
    std::cout << "  HidingLogger.log(5) -> " << hiding.log(5) << "  <- int -> double, Logger::log(int) se ne vidi\n";
    std::cout << "  FileLogger.log(5)   -> " << file.log(5) << "\n";
    std::cout << "  FileLogger.log(\"x\") -> " << file.log(std::string("x")) << "\n";
}

// ---------------------------------------------------------------- 4
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;            // pure virtual: Shape je apstraktna (errors/e02)
    virtual std::string name() const { return "Shape"; }
    std::string describe() const {               // nije virtual, ali poziva virtual funkcije
        return name() + " povrsine " + std::to_string(static_cast<int>(area()));
    }
};

class Circle : public Shape {
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return 3.14159 * r_ * r_; } // override: kompajler proveri potpis (errors/e03)
    std::string name() const override { return "Circle"; }

private:
    double r_;
};

class Square final : public Shape { // final: od Square se ne može dalje nasleđivati (errors/e04)
public:
    explicit Square(double side) : side_(side) {}
    double area() const override { return side_ * side_; }
    std::string name() const override { return "Square"; }

private:
    double side_;
};

void s04_virtualDispatch() {
    std::cout << "-- 4. virtual, override, final --\n";
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(2.0));
    shapes.push_back(std::make_unique<Square>(3.0));
    for (const auto& s : shapes) {
        std::cout << "  " << s->describe() << "\n"; // poziva se funkcija STVARNOG tipa objekta
    }
    Circle c(1.0);
    const Shape& ref = c;
    std::cout << "  kroz referencu: ref.name()=" << ref.name()
              << ", kvalifikovano ref.Shape::name()=" << ref.Shape::name() << "  <- kvalifikacija isključuje virtual\n";
}

// ---------------------------------------------------------------- 5
class Resource {
public:
    virtual ~Resource() { std::cout << "~Resource() "; } // virtual destruktor (EC++ Item 7)
};

class FileResource : public Resource {
public:
    ~FileResource() override { std::cout << "~FileResource() "; }
};

void s05_virtualDestructor() {
    std::cout << "-- 5. virtual destruktor --\n";
    std::cout << "  delete kroz Resource*: ";
    Resource* r = new FileResource;
    delete r; // zbog virtual ~Resource() poziva se prvo ~FileResource (bez njega: ub/u01)
    std::cout << "\n";
}

// ---------------------------------------------------------------- 6
class Widget {
public:
    Widget() { std::cout << "u Widget(): " << kind() << "; "; } // tokom konstrukcije baze objekat JE Widget
    virtual ~Widget() = default;
    virtual std::string kind() const { return "Widget::kind"; }
    void show() const { std::cout << "posle konstrukcije: " << kind(); }
};

class Button : public Widget {
public:
    Button() : label_("OK") {}
    std::string kind() const override { return "Button::kind(" + label_ + ")"; } // koristi label_

private:
    std::string label_;
};

void s06_virtualInConstructor() {
    std::cout << "-- 6. virtual poziv u konstruktoru (EC++ Item 9) --\n";
    std::cout << "  ";
    Button b;
    b.show();
    std::cout << "\n  <- u konstruktoru baze Button deo još ne postoji (label_ nije napravljen)\n";
}

// ---------------------------------------------------------------- 7
class Plain {
public:
    int x() const { return x_; }

private:
    int x_ = 0;
};

class WithVirtual {
public:
    virtual ~WithVirtual() = default;
    int x() const { return x_; }

private:
    int x_ = 0;
};

void s07_vtable() {
    std::cout << "-- 7. cena: vptr u svakom objektu --\n";
    std::cout << "  sizeof(Plain)=" << sizeof(Plain) << " sizeof(WithVirtual)=" << sizeof(WithVirtual)
              << "  <- skriveni pokazivač na tabelu virtual funkcija (+ poravnanje)\n";
    (void)Plain{}.x();
    (void)WithVirtual{}.x();
}

// ---------------------------------------------------------------- 8
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

std::string byValue(Animal a) { return a.speak(); }      // NE RADI OVAKO: kopira samo Animal deo
std::string byRef(const Animal& a) { return a.speak(); } // bez kopije: pravi tip ostaje

void s08_slicing() {
    std::cout << "-- 8. slicing --\n";
    Dog rex("Rex");
    std::cout << "  byValue(rex): " << byValue(rex) << "   <- odsečeno na Animal\n";
    std::cout << "  byRef(rex):   " << byRef(rex) << "\n";

    Animal copy = rex; // kopija u bazni objekat -- isto
    std::cout << "  Animal copy = rex; copy.speak(): " << copy.speak() << "\n";

    std::vector<Animal> zoo;
    zoo.push_back(rex); // kontejner baznih objekata čuva samo bazne delove
    std::cout << "  vector<Animal>[0].speak(): " << zoo[0].speak() << "\n";

    std::vector<std::unique_ptr<Animal>> realZoo; // ispravno: čuvaj pokazivače
    realZoo.push_back(std::make_unique<Dog>("Max"));
    std::cout << "  vector<unique_ptr<Animal>>[0]->speak(): " << realZoo[0]->speak() << "\n";
    // Da je Animal zabranio kopiranje (C.67), byValue i Animal copy = rex se
    // ne bi kompajlirali (errors/e01).
}

// ---------------------------------------------------------------- 9
class Document {
public:
    virtual ~Document() = default;
    Document& operator=(const Document&) = delete; // dodela bi odsekla (C.67)
    virtual std::unique_ptr<Document> clone() const = 0; // "virtual copy konstruktor" (C.130)
    virtual std::string kind() const = 0;

protected:
    Document() = default;
    Document(const Document&) = default; // samo izvedene klase smeju da kopiraju bazni deo
};

class Report : public Document {
public:
    explicit Report(std::string title) : title_(std::move(title)) {}
    std::unique_ptr<Document> clone() const override { return std::make_unique<Report>(*this); }
    std::string kind() const override { return "Report(" + title_ + ")"; }

private:
    std::string title_;
};

void s09_clone() {
    std::cout << "-- 9. kopiranje polimorfnog objekta: clone() --\n";
    std::unique_ptr<Document> original = std::make_unique<Report>("Q3");
    std::unique_ptr<Document> copy = original->clone(); // kopija PRAVOG tipa, bez slicing-a
    std::cout << "  original->clone()->kind() = " << copy->kind() << "\n";
}

// ---------------------------------------------------------------- 10
struct Device {
    explicit Device(int id) : id_(id) { std::cout << "Device(" << id << ") "; }
    int id_;
};
// virtual nasleđivanje: Printer i Scanner dele JEDAN Device podobjekat.
struct Printer : virtual Device {
    Printer() : Device(1) {}
};
struct Scanner : virtual Device {
    Scanner() : Device(2) {}
};
struct Copier : Printer, Scanner {
    Copier() : Device(3) {} // virtual bazu pravi NAJIZVEDENIJA klasa; Device(1) i Device(2) se ignorišu
};

void s10_multipleInheritance() {
    std::cout << "-- 10. višestruko nasleđivanje i dijamant --\n";
    std::cout << "  ";
    Copier c;
    std::cout << "| c.id_=" << c.id_ << " (jedan Device; bez virtual bi bila dva i c.id_ je dvosmisleno, errors/e12)\n";
}

// ---------------------------------------------------------------- 11
class Engine {
public:
    std::string start() const { return "motor radi"; }
};

class Car { // kompozicija: Car IMA Engine (EC++ Item 38) -- obično bolje od private nasleđivanja
public:
    std::string drive() const { return engine_.start() + ", auto vozi"; }

private:
    Engine engine_;
};

class Scooter : private Engine { // private nasleđivanje: "implementirano pomoću" (EC++ Item 39)
public:
    std::string ride() const { return start() + ", trotinet vozi"; }
};

void s11_compositionVsPrivate() {
    std::cout << "-- 11. kompozicija vs private nasleđivanje --\n";
    Car car;
    Scooter scooter;
    std::cout << "  Car::drive(): " << car.drive() << "; Scooter::ride(): " << scooter.ride()
              << "  (Engine& e = scooter; se ne kompajlira, errors/e07)\n";
}

int main() {
    s01_basics();
    s02_constructionOrder();
    s03_nameHiding();
    s04_virtualDispatch();
    s05_virtualDestructor();
    s06_virtualInConstructor();
    s07_vtable();
    s08_slicing();
    s09_clone();
    s10_multipleInheritance();
    s11_compositionVsPrivate();
}
